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
  
    //Connect pointers on the graph (simple)
  DEBUG_WITH_TYPE("phases", errs() << "  - Connecting pointers on the graph\n");
  
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
