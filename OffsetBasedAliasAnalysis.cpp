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

// Project's includes
#include "OffsetBasedAliasAnalysis.h"
#include "Offset.h"
// LLVM's includes
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Debug.h"
#include "llvm/ADT/Statistic.h"
// libc's includes
#include <ctime>

//Statistics and debug
#define DEBUG_TYPE "obaa"
STATISTIC(NInst, "Number of instructions");
STATISTIC(NPointers, "Number of Pointers");

using namespace llvm;

/// Globals
//...

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

/// \brief LLVM framework Main Function. Does the main analysis.
bool OffsetBasedAliasAnalysis::runOnModule(Module &M)
{
  //Enviroment setup. This includes seting up globals, stats 
  //and offset implementations
  
  clock_t t;
  t = clock();
  Offset::initRepresentations();
  
  //Build the pointer dependence graph  

    //Gather all pointers and stores

    //Connect pointers on the graph (simple)

    //Connect loads to stored pointers (complex)

    //Get narrowing information
  
  //Store local trees

  //Graph transformation
  
      //Find SCCs
      
      //Resolve SCCs
      
      //Resolve whole graph
      
      //Apply widening
      
      //Apply narrowing
      
      //Add self to base pointers
      
  //Finish enviroment
  
  return false;
}
