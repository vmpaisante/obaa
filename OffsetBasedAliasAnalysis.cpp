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
#include "Offset.h"
#include "OffsetPointer.h"
// llvm includes
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
// STL includes
#include <set>

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
  
  
  t = clock() - t;
  errs() << " Total time: " << (((float)t)/CLOCKS_PER_SEC) << "\n";
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

/// \brief Function that prints the dependence graph in DOT format
void OffsetBasedAliasAnalysis::printDOT(Module &M, std::string Stage) { }
