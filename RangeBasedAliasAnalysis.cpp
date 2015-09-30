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
#define DEBUG_TYPE "rbaa"

// Project's includes
#include "RangeBasedAliasAnalysis.h"
// LLVM's includes
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Debug.h"
// libc's includes
//...

using namespace llvm;

/// Globals
//...

/// \brief LLVM framework support function
void RangeBasedAliasAnalysis::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.setPreservesAll();
  //AU.addRequired<...>();
}
char RangeBasedAliasAnalysis::ID = 0;
static RegisterPass<RangeBasedAliasAnalysis> X("rbaa", 
	"Range Based Alias Analysis", false, false);

/// \brief LLVM framework Main Function. Does the main analysis.
bool RangeBasedAliasAnalysis::runOnModule(Module &M)
{
  return false;
}
