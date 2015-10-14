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

// Project's includes
#include "OffsetBasedAliasAnalysis.h"
#include "Offset.h"
#include "RangedPointer.h"
#include "Narrowing.h"
#include "Address.h"
#include "Primitives.h"
// LLVM's includes
#include "llvm/Pass.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/Debug.h"
#include "llvm/ADT/Statistic.h"
// libc's includes
#include <ctime>
#include <iostream>
#include <fstream>
#include <system_error>

//Statistics and debug
#define DEBUG_TYPE "obaa"
STATISTIC(NInst, "Number of instructions");
STATISTIC(NPointers, "Number of Pointers");
STATISTIC(NUnknown, "Number of Unknown Pointers");

using namespace llvm;

/// Globals
//...

/// \brief OffsetBasedaliasAnalysis class implementation

// Prints a DOT graph of the dependence graph
void OffsetBasedAliasAnalysis::printDOT(Module &M, std::string stage){}

//Function that gathers the module's pointers
void OffsetBasedAliasAnalysis::gatherPointers(Module &M)
{
	/// Go through global variables to find arrays, structs and pointers
  for(auto i = M.global_begin(), e = M.global_end(); i != e; i++)
    //Since all globals are pointers, all are inserted
    AllPointers.insert(i);
  /// Go through all functions from the module
  for (auto F = M.begin(), Fe = M.end(); F != Fe; F++)
  {
    /// Go through parameters (add if they are pointers)
    for(auto i = F->arg_begin(), e = F->arg_end(); i != e; i++)
    {
      Type* const arg_type = i->getType();
      if(arg_type->isPointerTy())
        AllPointers.insert(i);
    }
     /// Run through instructions from function
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I)
    {
      NInst++;
      const Instruction* i = &(*I);
      const Type *type = i->getType();
      if(type->isPointerTy())
        AllPointers.insert(i);
      else if(const StoreInst* str_int = dyn_cast<StoreInst>(i))
      {
        const Type *type2 = str_int->getValueOperand()->getType();
        if(type2->isPointerTy())
          RelevantStores.insert(str_int);
      }
      //verify intruction operands
      for(auto oi = i->op_begin(), oe = i->op_end(); oi != oe; oi++)
      {
        const Value* oper = *oi;
        const Type *op_type = oper->getType();
        if(op_type->isPointerTy())
          AllPointers.insert(oper);
      }
    }
  }
}

// Builds the pointer dependence graph
void OffsetBasedAliasAnalysis::simpleConnect()
{
	NPointers = AllPointers.size();
  for(auto *i : AllPointers)
  {
    RangedPointers[i] = new RangedPointer(i);
  }
  for(auto i : RangedPointers)
  {
    i.second->processInitialAddresses(this);
  }
  
  for(auto i : RangedPointers)
  {
    if(i.second->addr_empty())
      if(i.second->getPointerType() == RangedPointer::Cont
      or i.second->getPointerType() == RangedPointer::Phi)
        i.second->setPointerType(RangedPointer::Unk);
  }
}

// Function that collects narrowing information
void OffsetBasedAliasAnalysis::getNarrowingData()
{
  std::map<Value*, NarrowingData*> narrowing_data;
  //getting narrowing information
  for(auto i : RangedPointers)
  {
    if(const PHINode* phi = dyn_cast<PHINode>(i.first))
    {
      if (phi->getName().startswith("SSIfy_sigma")) 
      {
        //find branch
        BasicBlock* o_block = phi->getIncomingBlock(0);
        BasicBlock::iterator bi = o_block->end();
        bi--;
        BranchInst* br = (BranchInst*) &(*bi);
        Value* conditional = br->getCondition();
        ICmpInst* cmp_i;
        if(isa<ICmpInst>(*conditional)) cmp_i = (ICmpInst*) conditional;
        else continue;
        if(narrowing_data[br] == NULL)
        {
          narrowing_data[br] = new NarrowingData();
          narrowing_data[br]->cmp_v1 = cmp_i->getOperand(0);
          narrowing_data[br]->cmp_v2 = cmp_i->getOperand(1);
          narrowing_data[br]->cmp_op = cmp_i->getPredicate();
        }
        narrowing_data[br]->sigmas.insert(phi);       
      }
    }
  }
  
  //Associating narrowing ops
  
  for(auto i : narrowing_data)
  {
    BranchInst* br = (BranchInst*) i.first;
    ICmpInst* cmp_i = (ICmpInst*) br->getCondition();
    //for each sigma
    for(std::set<const PHINode*>::iterator ii = i.second->sigmas.begin(), 
    ee = i.second->sigmas.end(); ii != ee; ii++)
    {
      const PHINode* sigma = (const PHINode*) *ii;
      NarrowingOp* no = new NarrowingOp();
      if(i.second->cmp_op == CmpInst::ICMP_EQ)
      {
        if(sigma->getParent() == br->getSuccessor(0))
        { //its the true sigma
          no->cmp_op = CmpInst::ICMP_EQ;
          if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(0))
          { //its the left operand of cmp
            no->cmp_v = cmp_i->getOperand(1);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1))
          { //its the right operand of cmp
            no->cmp_v = cmp_i->getOperand(0);
          }  
        }
        else if(sigma->getParent() == br->getSuccessor(1))
        { //its the false sigma
          no->cmp_op = CmpInst::ICMP_NE;
          if(sigma->getIncomingValue(0) == cmp_i->getOperand(0))
          { //its the left operand of cmp
            no->cmp_v = cmp_i->getOperand(1);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1))
          { //its the right operand of cmp
            no->cmp_v = cmp_i->getOperand(0);
          } 
        }
      }
      else if(i.second->cmp_op == CmpInst::ICMP_NE)
      {
      	if(sigma->getParent() == br->getSuccessor(0))
        { //its the true sigma
          no->cmp_op = CmpInst::ICMP_NE;
          if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(0))
          { //its the left operand of cmp
            no->cmp_v = cmp_i->getOperand(1);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1))
          { //its the right operand of cmp
            no->cmp_v = cmp_i->getOperand(0);
          }  
        }
        else if(sigma->getParent() == br->getSuccessor(1))
        { //its the false sigma
          no->cmp_op = CmpInst::ICMP_EQ;
          if(sigma->getIncomingValue(0) == cmp_i->getOperand(0))
          { //its the left operand of cmp
            no->cmp_v = cmp_i->getOperand(1);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1))
          { //its the right operand of cmp
            no->cmp_v = cmp_i->getOperand(0);
          } 
        }
      }
      else if(i.second->cmp_op == CmpInst::ICMP_UGT 
      or i.second->cmp_op == CmpInst::ICMP_SGT)
      {
      	if(sigma->getParent() == br->getSuccessor(0))
        { //its the true sigma
          if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(0))
          { //its the left operand of cmp
            no->cmp_v = cmp_i->getOperand(1);
            no->cmp_op = CmpInst::ICMP_SGT;
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1))
          { //its the right operand of cmp
            no->cmp_v = cmp_i->getOperand(0);
            no->cmp_op = CmpInst::ICMP_SLT;
          }  
        }
        else if(sigma->getParent() == br->getSuccessor(1))
        { //its the false sigma
          if(sigma->getIncomingValue(0) == cmp_i->getOperand(0))
          { //its the left operand of cmp
            no->cmp_op = CmpInst::ICMP_SLE;
			      no->cmp_v = cmp_i->getOperand(1);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1))
          { //its the right operand of cmp
            no->cmp_op = CmpInst::ICMP_SGE;
            no->cmp_v = cmp_i->getOperand(0);
          } 
        }
      }
      else if(i.second->cmp_op == CmpInst::ICMP_UGE 
      or i.second->cmp_op == CmpInst::ICMP_SGE)
      {
      	if(sigma->getParent() == br->getSuccessor(0))
        { //its the true sigma
          if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(0))
          { //its the left operand of cmp
            no->cmp_op = CmpInst::ICMP_SGE;
            no->cmp_v = cmp_i->getOperand(1);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1))
          { //its the right operand of cmp
            no->cmp_op = CmpInst::ICMP_SLE;
            no->cmp_v = cmp_i->getOperand(0);
          }  
        }
        else if(sigma->getParent() == br->getSuccessor(1))
        { //its the false sigma
          if(sigma->getIncomingValue(0) == cmp_i->getOperand(0))
          { //its the left operand of cmp
            no->cmp_op = CmpInst::ICMP_SLT;
            no->cmp_v = cmp_i->getOperand(1);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1))
          { //its the right operand of cmp
            no->cmp_op = CmpInst::ICMP_SGT;
            no->cmp_v = cmp_i->getOperand(0);
          } 
        }
      }
      else if(i.second->cmp_op == CmpInst::ICMP_ULT 
      or i.second->cmp_op == CmpInst::ICMP_SLT)
      {
      	if(sigma->getParent() == br->getSuccessor(0))
        { //its the true sigma
          if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(0))
          { //its the left operand of cmp
            no->cmp_op = CmpInst::ICMP_SLT;
            no->cmp_v = cmp_i->getOperand(1);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1))
          { //its the right operand of cmp
            no->cmp_op = CmpInst::ICMP_SGT;
            no->cmp_v = cmp_i->getOperand(0);
          }  
        }
        else if(sigma->getParent() == br->getSuccessor(1))
        { //its the false sigma
          if(sigma->getIncomingValue(0) == cmp_i->getOperand(0))
          { //its the left operand of cmp
            no->cmp_op = CmpInst::ICMP_SGE;
            no->cmp_v = cmp_i->getOperand(1);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1))
          { //its the right operand of cmp
            no->cmp_op = CmpInst::ICMP_SLE;
            no->cmp_v = cmp_i->getOperand(0);
          } 
        }
      }
      else if(i.second->cmp_op == CmpInst::ICMP_ULE 
      or i.second->cmp_op == CmpInst::ICMP_SLE)
      {
      	if(sigma->getParent() == br->getSuccessor(0))
        { //its the true sigma
          if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(0))
          { //its the left operand of cmp
            no->cmp_op = CmpInst::ICMP_SLE;
            no->cmp_v = cmp_i->getOperand(1);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1))
          { //its the right operand of cmp
            no->cmp_op = CmpInst::ICMP_SGE;
            no->cmp_v = cmp_i->getOperand(0);
          }  
        }
        else if(sigma->getParent() == br->getSuccessor(1))
        { //its the false sigma
          if(sigma->getIncomingValue(0) == cmp_i->getOperand(0))
          { //its the left operand of cmp
            no->cmp_op = CmpInst::ICMP_SGT;
            no->cmp_v = cmp_i->getOperand(1);
          }
          else if(sigma->getIncomingValue(0) ==  cmp_i->getOperand(1))
          { //its the right operand of cmp
            no->cmp_op = CmpInst::ICMP_SLT;
            no->cmp_v = cmp_i->getOperand(0);
          } 
        }
      }
      
      //add narrowing op to sigma's ranged pointer
      for(std::set<Address*>::iterator ii = RangedPointers[sigma]->addr_begin(),
      ee = RangedPointers[sigma]->addr_end(); ii != ee; ii++)
      {
        (*ii)->associateNarrowingOp(sigma, no);
      }
    } 
  }
}

/// \brief LLVM framework support function
void OffsetBasedAliasAnalysis::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.setPreservesAll();
  //AU.addRequired<...>();
  Offset::getAnalysisUsage(AU);
}
char OffsetBasedAliasAnalysis::ID = 0;
static RegisterPass<OffsetBasedAliasAnalysis> X("obaa", 
	"Offset Based Alias Analysis", false, false);
static RegisterAnalysisGroup<AliasAnalysis> E(X);

/// \brief LLVM's AliasAnalysis framework Functions.
OffsetBasedAliasAnalysis::AliasResult 
OffsetBasedAliasAnalysis::alias(const Location &LocA, const Location &LocB)
{

}

bool OffsetBasedAliasAnalysis::pointsToConstantMemory
(const Location &Loc, bool OrLocal)
{

}

/// \brief LLVM framework Main Function. Does the main analysis.
bool OffsetBasedAliasAnalysis::runOnModule(Module &M)
{
  //Enviroment setup. This includes seting up globals, stats 
  //and offset implementations
  DEBUG_WITH_TYPE("phases", errs() << "=> Setting up\n");
  
  clock_t t;
  t = clock();
  Offset::initRepresentations();
  
  //Build the pointer dependence graph  
  DEBUG_WITH_TYPE("phases", errs() << "=> Building dependence graph\n");
  
    //Gather all pointers and stores
  DEBUG_WITH_TYPE("phases", errs() << "  - Gathering module's pointers\n");
  gatherPointers(M);
  
    //Connect pointers on the graph (simple)
  DEBUG_WITH_TYPE("phases", errs() << "  - Connecting pointers on the graph\n");
  simpleConnect();
  
    //Connect loads to stored pointers (complex)
  DEBUG_WITH_TYPE("phases", errs() << "  - Connecting load and stores\n");
  
    //Get narrowing information
  DEBUG_WITH_TYPE("phases", errs() << "  - Getting narrowing information\n");
  getNarrowingData();
  
  DEBUG_WITH_TYPE("dot_graphs", printDOT(M, std::string("_Before")));  
  //Store local trees
  DEBUG_WITH_TYPE("phases", errs() << "=> Storing local trees\n");

  //Graph transformation
  DEBUG_WITH_TYPE("phases", errs() << "=> Normalizing graph\n");
      //Find SCCs
  DEBUG_WITH_TYPE("phases", errs() << "  - Finding SCCs\n");
      
      //Resolve SCCs
  DEBUG_WITH_TYPE("phases", errs() << "  - Resolving SCCs\n");
      
      //Resolve whole graph
  DEBUG_WITH_TYPE("phases", errs() << "  - Resolving whole graph\n");
      
      //Apply widening
  DEBUG_WITH_TYPE("phases", errs() << "  - Applying widening\n");
      
      //Apply narrowing
  DEBUG_WITH_TYPE("phases", errs() << "  - Applying narrowing\n");
      
  DEBUG_WITH_TYPE("dot_graphs", printDOT(M, std::string("_After"))); 
      //Add self to base pointers
  DEBUG_WITH_TYPE("phases", errs() << "  - Adding self to base pointers\n");
  
  //Finish enviroment
  DEBUG_WITH_TYPE("phases", errs() << "=> Finishing up\n");
  
  return false;
}
