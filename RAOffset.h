//===-------------------- RAOffset.h - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the RAOffset class. It is an offset
/// representation that uses a numerical Range Analysis 
/// 
///
//===----------------------------------------------------------------------===//

#ifndef __RAOFFSET_H__
#define __RAOFFSET_H__

// project's includes
#include "Offset.h"
#include "RangeAnalysis.h"
// llvm's includes
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/raw_ostream.h"
// libc includes
#include <map>

namespace llvm {

// Forward declarations
class AnalysisUsage;
struct NarrowingOp;
struct WideningOp;
class Value;
class OffsetPointer;
class OffsetBasedAliasAnalysis;

/// \brief Offset representation that uses range analysis
class RAOffset : public OffsetRepresentation {
  
public:
  RAOffset();
  
  /// \brief Builds \p pointer's offset using \p base 
  RAOffset(const Value* Pointer, const Value* Base);
  
  /// \brief Destructor 
  ~RAOffset() override;
  
  /// \brief Returns a copy of the represented offset
  RAOffset* copy() override;
  
  /// \brief Adds two offsets of the respective representation
  RAOffset* add(OffsetRepresentation* Other) override;
  
  /// \brief Answers true if two offsets are disjoints
  bool disjoint(OffsetRepresentation* Other) override;
  
  /// \brief Narrows the offset of the respective representation
  RAOffset* narrow(CmpInst::Predicate Cmp, OffsetRepresentation* Other) override;
  
  /// \brief Widens the offset of the respective representation, Before and 
  ///   After are given so its possible to calculate direction of growth.
  RAOffset* widen(OffsetRepresentation* Before, OffsetRepresentation* After) override;
  
  /// \brief Prints the offset representation
  void print() override;
  /// \brief Prints the offset to a file
  void print(raw_fd_ostream& fs) override;

  static void initialization(OffsetBasedAliasAnalysis* Analysis);

private:
  static InterProceduralRA<Cousot>* ra;
  Range r;
};

}

#endif
