//===----------------- RAOffset.cpp - Pass definition -----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief
///
//===----------------------------------------------------------------------===//

// project's includes
#include "Offset.h"
#include "RAOffset.h"
#include "OffsetBasedAliasAnalysis.h"
// llvm's includes
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Pass.h"
// libc includes
#include <map>

using namespace llvm;

extern APInt Zero;

RAOffset::RAOffset() : r(Zero, Zero) { }

/// \brief Builds \p pointer's offset using \p base 
RAOffset::RAOffset(const Value* Pointer, const Value* Base) { }

/// \brief Destructor 
RAOffset::~RAOffset() { }

/// \brief Returns a copy of the represented offset
RAOffset* RAOffset::copy() { return NULL; }

/// \brief Adds two offsets of the respective representation
RAOffset* RAOffset::add(OffsetRepresentation* Other) { return NULL; }

/// \brief Answers true if two offsets are disjoints
bool RAOffset::disjoint(OffsetRepresentation* Other) { return false; }

/// \brief Narrows the offset of the respective representation
RAOffset* RAOffset::narrow(CmpInst::Predicate Cmp, OffsetRepresentation* Other){
  return NULL;
}

/// \brief Widens the offset of the respective representation, Before and 
///   After are given so its possible to calculate direction of growth.
RAOffset* RAOffset::widen(OffsetRepresentation* Before,
OffsetRepresentation* After) { return NULL; }

/// \brief Prints the offset representation
void RAOffset::print() { }
/// \brief Prints the offset to a file
void RAOffset::print(raw_fd_ostream& fs) { }


void RAOffset::initialization(OffsetBasedAliasAnalysis* Analysis) {
  ra = &(Analysis->getAnalysis<IntraProceduralRA<Cousot> >());
}
