//===--- OffsetBasedAliasAnalysis.h - Pass definition ----------*- C++ -*-===//
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
#ifndef __OFFSET_BASED_ALIAS_ANALYSIS_H__
#define __OFFSET_BASED_ALIAS_ANALYSIS_H__

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
#include <set>

namespace llvm
{

/// Forward declarations
class RangedPointer;

class OffsetBasedAliasAnalysis : public ModulePass, public AliasAnalysis
{
  public:
  
  /// LLVM framework methods and atributes
  static char ID; // Class identification, replacement for typeinfo
  OffsetBasedAliasAnalysis() : ModulePass(ID){}
  void getAnalysisUsage(AnalysisUsage &AU) const override;
  bool runOnModule(Module &M) override;
  
  /// Alias Analysis framework methods
  AliasResult alias(const Location &LocA, const Location &LocB) override;
  bool pointsToConstantMemory(const Location &Loc, bool OrLocal) override;
  /// getAdjustedAnalysisPointer - This method is used when a pass implements
  /// an analysis interface through multiple inheritance.  If needed, it
  /// should override this to adjust the this pointer as needed for the
  /// specified pass info.
  virtual void *getAdjustedAnalysisPointer(AnalysisID PI) {
    if (PI == &AliasAnalysis::ID)
      return (AliasAnalysis*)this;
    return this;
  }
  
  // Function that returns the ranged pointer corresponding
  //  to the value given
  RangedPointer* getRangedPointer(const Value*);
  
  private:
  std::map<const Value*, RangedPointer*> RangedPointers;
  std::set<const Value*> AllPointers;
  std::set<const StoreInst*> RelevantStores;
  void gatherPointers(Module &M);
  void simpleConnect();
  void getNarrowingData();
  void printDOT(Module &M, std::string stage);
};

}

#endif
