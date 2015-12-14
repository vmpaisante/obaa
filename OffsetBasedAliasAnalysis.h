//===--- OffsetBasedAliasAnalysis.h - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the Offset Based Alias Analy-
/// sis pass. TODO: Better description
///
//===----------------------------------------------------------------------===//
#ifndef __OFFSET_BASED_ALIAS_ANALYSIS_H__
#define __OFFSET_BASED_ALIAS_ANALYSIS_H__

// LLVM's includes
#include "llvm/Pass.h"
#include "llvm/Analysis/AliasAnalysis.h"
// libc's includes
#include <map>
#include <set>

namespace llvm
{

/// Forward declarations
class OffsetPointer;

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
  
  /// \brief Function that returns the offset pointer corresponding
  ///  to the value given
  OffsetPointer* getOffsetPointer(const Value*);
  
private:
  /// \brief map that contains all the pointers represented
  std::map<const Value*, OffsetPointer* const> offset_pointers;
  std::set<const Value*> all_pointers;
  std::set<const StoreInst*> relevant_stores;
  /// \brief Gather all pointers from the module
  void gatherPointers(Module &M);
  /// \brief Connects the offset pointers by just looking at them
  void simpleConnect();
  /// \brief Gets the data used for narrowing operations from the module
  void getNarrowingData();
  /// \brief Function that prints the dependence graph in DOT format
  void printDOT(Module &M, std::string Stage);
};

}

#endif
