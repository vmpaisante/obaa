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
#include "RangeAnalysis.h"
// llvm's includes
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Pass.h"
// libc includes
#include <map>

using namespace llvm;

extern unsigned MAX_BIT_INT;
extern APInt Min;
extern APInt Max;
extern APInt Zero;

RAOffset::RAOffset() : r(Zero, Zero) { }

/// \brief Builds \p pointer's offset using \p base 
RAOffset::RAOffset(const Value* Pointer, const Value* Base) : r(Zero, Zero) { }

/// \brief Destructor 
RAOffset::~RAOffset() { }

/// \brief Returns a copy of the represented offset
RAOffset* RAOffset::copy() {
  RAOffset* copy = new RAOffset();
  copy->r = r;
  return copy;
}

/// \brief Adds two offsets of the respective representation
RAOffset* RAOffset::add(OffsetRepresentation* Other) { return new RAOffset(); }

/// \brief Answers true if two offsets are disjoints
bool RAOffset::disjoint(OffsetRepresentation* Other) { return false; }

/// \brief Narrows the offset of the respective representation
RAOffset* RAOffset::narrow(CmpInst::Predicate Cmp, OffsetRepresentation* Other){
  return new RAOffset();
}

/// \brief Widens the offset of the respective representation, Before and 
///   After are given so its possible to calculate direction of growth.
RAOffset* RAOffset::widen(OffsetRepresentation* Before,
OffsetRepresentation* After) { return new RAOffset(); }

/// \brief Prints the offset representation
void RAOffset::print() { r.print(errs()); }
/// \brief Prints the offset to a file
void RAOffset::print(raw_fd_ostream& fs) { r.print(fs); }


void RAOffset::initialization(OffsetBasedAliasAnalysis* Analysis) {
  RAOffset::ra = &(Analysis->getAnalysis<InterProceduralRA<Cousot> >());
}
