//===--- RangeBasedAliasAnalysis.h - Pass definition ----------*- C++ -*-===//
//
//             Range Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the Range Based Alias Analy-
/// sis pass.
///
//===----------------------------------------------------------------------===//
#ifndef __RANGE_BASED_ALIAS_ANALYSIS_H__
#define __RANGE_BASED_ALIAS_ANALYSIS_H__

#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Pass.h"

namespace llvm
{

/// Forward declarations
//...

class RangeBasedAliasAnalysis : public ModulePass, public AliasAnalysis
{
  public:
  
  /// LLVM framework methods and atributes
  static char ID; // Class identification, replacement for typeinfo
  RangeBasedAliasAnalysis() : ModulePass(ID){}
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
  
};

}

#endif
