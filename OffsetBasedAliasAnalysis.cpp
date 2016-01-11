//===-- OffsetBasedAliasAnalysis.cpp - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the Offset Based Alias Analy-
/// sis pass.
///
//===----------------------------------------------------------------------===//
#define DEBUG_TYPE "obaa"

// local includes
#include "OffsetBasedAliasAnalysis.h"
#include "Address.h"
#include "Narrowing.h"
#include "Offset.h"
#include "OffsetPointer.h"
// llvm includes
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/User.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
// STL includes
#include <set>
#include <string>
#include <map>

STATISTIC(NumPointers, "Number of pointers from the module");
STATISTIC(NumRelevantStores, "Number of relevant stores from the module");

using namespace llvm;

/// LLVM framework methods and atributes
char OffsetBasedAliasAnalysis::ID = 0;

static RegisterPass<OffsetBasedAliasAnalysis> X("obaa",
"Offset based Alias Analysis", false, false);

static RegisterAnalysisGroup<AliasAnalysis> E(X);

void OffsetBasedAliasAnalysis::getAnalysisUsage(AnalysisUsage &AU) const {
  AliasAnalysis::getAnalysisUsage(AU);
  Offset::getAnalysisUsage(AU);
  AU.setPreservesAll();
}

bool OffsetBasedAliasAnalysis::runOnModule(Module &M) {
  //Settin up
  InitializeAliasAnalysis(this);
  clock_t t;
  t = clock();
  
  DEBUG_WITH_TYPE("phases", errs() << "Gathering module's pointers\n");
  /// The first step of the program consists on 
  /// gathering all pointers and stores
  gatherPointers(M);
  
  DEBUG_WITH_TYPE("phases", errs() << "Building intra dependence graph\n");
  /// The second step of the program consists in building
  /// the intra procedural pointer dependence graph
  buildIntraProceduralDepGraph();
  
  DEBUG_WITH_TYPE("phases", errs() << "Getting narrowing information\n");
  /// Getting narrowing information
  getNarrowingInfo();
  
  DEBUG_WITH_TYPE("dot_graphs", printDOT(M, std::string("_intra1")));
  
  t = clock() - t;
  errs() << "Total time: " << (((float)t)/CLOCKS_PER_SEC) << "\n";
  return false;
}

/// Alias Analysis framework methods
AliasAnalysis::AliasResult OffsetBasedAliasAnalysis::alias(const Location &LocA, 
const Location &LocB) {
  return AliasAnalysis::alias(LocA, LocB);
}

bool OffsetBasedAliasAnalysis::pointsToConstantMemory(const Location &Loc, 
bool OrLocal) {
  return AliasAnalysis::pointsToConstantMemory(Loc, OrLocal);
}

/// \brief Function that returns the offset pointer corresponding
///  to the value given
OffsetPointer* OffsetBasedAliasAnalysis::getOffsetPointer(const Value* V) {
  const Type *type = V->getType();
  // TODO: add assertion
  if(!type->isPointerTy())
    return NULL;
  if(offset_pointers[V] == NULL)
    offset_pointers[V] = new OffsetPointer(V, OffsetPointer::Unk);
  return offset_pointers[V];
}

/// \brief Gather all pointers from the module
void OffsetBasedAliasAnalysis::gatherPointers(Module &M) {
  /// Go through global variables to find arrays, structs and pointers
  for(auto i = M.global_begin(), e = M.global_end(); i != e; i++)
    //Since all globals are pointers, all are inserted
    all_pointers.insert(i);
  /// Go through all functions from the module
  for (auto F = M.begin(), Fe = M.end(); F != Fe; F++) {
    /// Go through parameters (add if they are pointers)
    for(auto i = F->arg_begin(), e = F->arg_end(); i != e; i++) {
      Type* const arg_type = i->getType();
      if(arg_type->isPointerTy())
        all_pointers.insert(i);
    }
     /// Run through instructions from function
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
      const Instruction* i = &(*I);
      const Type *type = i->getType();
      if(type->isPointerTy())
        all_pointers.insert(i);
      else if(const StoreInst* str_int = dyn_cast<StoreInst>(i)) {
        const Type *type2 = str_int->getValueOperand()->getType();
        if(type2->isPointerTy())
          relevant_stores.insert(str_int);
      }
      //verify intruction operands
      for(auto oi = i->op_begin(), oe = i->op_end(); oi != oe; oi++) {
        const Value* oper = *oi;
        const Type *op_type = oper->getType();
        if(op_type->isPointerTy())
          all_pointers.insert(oper);
      }
    }
  }
  
  NumPointers = all_pointers.size();
  NumRelevantStores = relevant_stores.size();
}

/// \brief Builds the dependence graph using an intra procedural frame
void OffsetBasedAliasAnalysis::buildIntraProceduralDepGraph() {
  for(auto *i : all_pointers) {
    offset_pointers[i] = new OffsetPointer(i);
  }
  for(auto i : offset_pointers) {
    i.second->addIntraProceduralAddresses(this);
    if(i.second->addr_empty())
      if(i.second->getPointerType() == OffsetPointer::Cont
      or i.second->getPointerType() == OffsetPointer::Phi)
        i.second->setPointerType(OffsetPointer::Unk);
  }
}

/// \brief Obtains narrowing information from the module
void OffsetBasedAliasAnalysis::getNarrowingInfo() {
  //getting narrowing information
  std::map<const Value*, NarrowingData> narrowing_data;
  for(auto i : offset_pointers) {
    if(const PHINode* phi = dyn_cast<PHINode>(i.first)) {
      if (phi->getName().startswith("SSIfy_sigma")) {
        //find branch
        BasicBlock* o_block = phi->getIncomingBlock(0);
        BasicBlock::iterator bi = o_block->end();
        bi--;
        BranchInst* br = (BranchInst*) &(*bi);
        Value* conditional = br->getCondition();
        ICmpInst* cmp_i;
        if(isa<ICmpInst>(*conditional)) cmp_i = (ICmpInst*) conditional;
        else continue;
        if(narrowing_data.find(br) == narrowing_data.end()) {
          NarrowingData nd = NarrowingData(cmp_i->getPredicate(), 
            cmp_i->getOperand(0), cmp_i->getOperand(1));
          narrowing_data.insert(std::pair<const Value*, NarrowingData>(br, nd));
        }
        narrowing_data.at(br).sigmas.insert(phi);
      }
    }
  }
  
  //Associating narrowing ops
  for(auto i : narrowing_data) {
    const BranchInst* br = (const BranchInst*) i.first;
    ICmpInst* cmp_i = (ICmpInst*) br->getCondition();
    //for each sigma
    for(std::set<const PHINode*>::iterator ii = i.second.sigmas.begin(), 
    ee = i.second.sigmas.end(); ii != ee; ii++) {
      const PHINode* sigma = (const PHINode*) *ii;
      NarrowingOp* no;
      if(i.second.cmp_op == CmpInst::ICMP_EQ) {
        if(sigma->getParent() == br->getSuccessor(0)) { 
          //its the true sigma
          if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(0)) { 
            //its the left operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_EQ, 
              offset_pointers[cmp_i->getOperand(1)]);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1)) { 
            //its the right operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_EQ, 
              offset_pointers[cmp_i->getOperand(0)]);
          }  
        }
        else if(sigma->getParent() == br->getSuccessor(1)) { 
          //its the false sigma
          if(sigma->getIncomingValue(0) == cmp_i->getOperand(0)) { 
            //its the left operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_NE, 
              offset_pointers[cmp_i->getOperand(1)]);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1)) { 
            //its the right operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_NE, 
              offset_pointers[cmp_i->getOperand(0)]);
          } 
        }
      }
      else if(i.second.cmp_op == CmpInst::ICMP_NE) {
      	if(sigma->getParent() == br->getSuccessor(0)) { 
          //its the true sigma
          if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(0)) { 
            //its the left operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_NE, 
              offset_pointers[cmp_i->getOperand(1)]);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1)) { 
            //its the right operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_NE, 
              offset_pointers[cmp_i->getOperand(0)]);
          }  
        }
        else if(sigma->getParent() == br->getSuccessor(1)) { 
          //its the false sigma
          if(sigma->getIncomingValue(0) == cmp_i->getOperand(0)) { 
            //its the left operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_EQ, 
              offset_pointers[cmp_i->getOperand(0)]);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1)) { 
            //its the right operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_EQ, 
              offset_pointers[cmp_i->getOperand(0)]);
          } 
        }
      }
      else if(i.second.cmp_op == CmpInst::ICMP_UGT 
      or i.second.cmp_op == CmpInst::ICMP_SGT) {
      	if(sigma->getParent() == br->getSuccessor(0)) { 
      	  //its the true sigma
          if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(0)) { 
            //its the left operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SGT, 
              offset_pointers[cmp_i->getOperand(1)]);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1)) { 
            //its the right operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SLT, 
              offset_pointers[cmp_i->getOperand(0)]);
          }  
        }
        else if(sigma->getParent() == br->getSuccessor(1)) { 
          //its the false sigma
          if(sigma->getIncomingValue(0) == cmp_i->getOperand(0)) { 
            //its the left operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SLE, 
              offset_pointers[cmp_i->getOperand(1)]);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1)) { 
            //its the right operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SGE, 
              offset_pointers[cmp_i->getOperand(0)]);
          } 
        }
      }
      else if(i.second.cmp_op == CmpInst::ICMP_UGE 
      or i.second.cmp_op == CmpInst::ICMP_SGE) {
      	if(sigma->getParent() == br->getSuccessor(0)) { 
      	  //its the true sigma
          if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(0)) { 
            //its the left operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SGE, 
              offset_pointers[cmp_i->getOperand(1)]);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1)) { 
            //its the right operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SLE, 
              offset_pointers[cmp_i->getOperand(0)]);
          }  
        }
        else if(sigma->getParent() == br->getSuccessor(1)) { 
          //its the false sigma
          if(sigma->getIncomingValue(0) == cmp_i->getOperand(0)) { 
            //its the left operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SLT, 
              offset_pointers[cmp_i->getOperand(1)]);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1)) { 
            //its the right operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SGT, 
              offset_pointers[cmp_i->getOperand(0)]);
          } 
        }
      }
      else if(i.second.cmp_op == CmpInst::ICMP_ULT 
      or i.second.cmp_op == CmpInst::ICMP_SLT) {
      	if(sigma->getParent() == br->getSuccessor(0)) { 
      	  //its the true sigma
          if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(0)) { 
            //its the left operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SLT, 
              offset_pointers[cmp_i->getOperand(1)]);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1)) { 
            //its the right operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SGT, 
              offset_pointers[cmp_i->getOperand(0)]);
          }  
        }
        else if(sigma->getParent() == br->getSuccessor(1)) { 
          //its the false sigma
          if(sigma->getIncomingValue(0) == cmp_i->getOperand(0)) { 
            //its the left operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SGE, 
              offset_pointers[cmp_i->getOperand(1)]);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1)) { 
            //its the right operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SLE, 
              offset_pointers[cmp_i->getOperand(0)]);
          } 
        }
      }
      else if(i.second.cmp_op == CmpInst::ICMP_ULE 
      or i.second.cmp_op == CmpInst::ICMP_SLE) {
      	if(sigma->getParent() == br->getSuccessor(0)) { 
      	  //its the true sigma
          if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(0)) { 
            //its the left operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SLE, 
              offset_pointers[cmp_i->getOperand(1)]);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1)) { 
            //its the right operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SGE, 
              offset_pointers[cmp_i->getOperand(0)]);
          }  
        }
        else if(sigma->getParent() == br->getSuccessor(1)) { 
          //its the false sigma
          if(sigma->getIncomingValue(0) == cmp_i->getOperand(0)) { 
            //its the left operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SGT, 
              offset_pointers[cmp_i->getOperand(1)]);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1)) { 
            //its the right operand of cmp
            no = new NarrowingOp(CmpInst::ICMP_SLT, 
              offset_pointers[cmp_i->getOperand(0)]);
          } 
        }
      }
      
      //add narrowing op to sigma's ranged pointer
      for(std::set<Address*>::iterator ii =offset_pointers[sigma]->addr_begin(),
      ee = offset_pointers[sigma]->addr_end(); ii != ee; ii++) {
        (*ii)->narrowing_ops.insert(std::pair<const Value*, const NarrowingOp>
          (sigma, *no));
      }
      delete no;
    }
  }
}

/// \brief Function that prints the dependence graph in DOT format
void OffsetBasedAliasAnalysis::printDOT(Module &M, std::string Stage) { 
  std::string name = M.getModuleIdentifier();
  name += Stage;
  name += ".dot";
  std::string er = "";
  raw_fd_ostream fs(name.data(), er, (sys::fs::OpenFlags)8);
  fs << "digraph grafico {\n";
  //printing nodes
  for(auto i : offset_pointers) {
    if(i.first->getValueName() == NULL)
      fs << "\"" << *(i.first) << "_" << i.first << "\" ";
    else
      fs << "\"" << (i.first->getName()) << "_" << i.first << "\" ";
    
    if(i.second->getPointerType() == OffsetPointer::Arg)
      fs << "[shape=egg];\n";
      else if(i.second->getPointerType() == OffsetPointer::Unk)
      fs << "[shape=plaintext];\n";
    else if(i.second->getPointerType() == OffsetPointer::Alloc)
      fs << "[shape=square];\n";
    else if(i.second->getPointerType() == OffsetPointer::Phi)
      fs << "[shape=diamond];\n";
    else if(i.second->getPointerType() == OffsetPointer::Cont)
      fs << "[shape=ellipse];\n";
    else if(i.second->getPointerType() == OffsetPointer::Null)
      fs << "[shape=point];\n";
      
    //printing edges
    for(std::set<Address*>::iterator j = i.second->addr_begin(),
    je = i.second->addr_end(); j != je; j++) {      
      if((*j)->getBase()->getPointer()->getValueName() == NULL)
        fs << "\"" << *((*j)->getBase()->getPointer()) << "_" << 
        (*j)->getBase()->getPointer() << "\" -> ";
      else
        fs << "\"" << ((*j)->getBase()->getPointer()->getName()) << "_" << 
        (*j)->getBase()->getPointer() << "\" -> ";
      
      if(i.first->getValueName() == NULL)
        fs << "\"" << *(i.first) << "_" << i.first << "\" [label=\"";
      else
        fs << "\"" << (i.first->getName()) << "_" << i.first << "\" [label=\"";
      
      if((*j)->wasWidened()) fs << "*";  
      (*j)->getOffset().print(fs);
      
      for(auto z : (*j)->narrowing_ops)
      {
        fs<< "N{";
        if(z.second.cmp_op == CmpInst::ICMP_EQ)
          fs << "=";
        else if(z.second.cmp_op == CmpInst::ICMP_NE)
          fs << "!=";
        else if(z.second.cmp_op == CmpInst::ICMP_SLT)
          fs << "<";
        else if(z.second.cmp_op == CmpInst::ICMP_SLE)
          fs << "<=";
        else if(z.second.cmp_op == CmpInst::ICMP_SGT)
          fs << ">";
        else if(z.second.cmp_op == CmpInst::ICMP_SGE)
          fs << ">=";
        fs << ", " << z.second.cmp_v << "+";
        z.second.context.print(fs); 
        fs << "]}";
      }
      
      fs << "\"";
      fs << "];\n";
    }
    
  }
  
  
  fs << "}";
}
