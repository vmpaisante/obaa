//===---------------------- Offset.h - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Offset class and
/// the offset representations base. An offset is part of a pointer 
/// representation and requires simple operations: Adding two offsets, 
/// checking if two offsets are disjoint, narrow an offset and widen it. 
/// 
///
//===----------------------------------------------------------------------===//

#ifndef __OFFSET_H__
#define __OFFSET_H__

// llvm's includes
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/raw_ostream.h"
// libc includes
#include <map>

namespace llvm {

// Forward declarations
class AnalysisUsage;
class NarrowingOp;
class WideningOp;
class Value;
class OffsetPointer;

/// \brief Abstract class for implementing offset representations.
/// Use it for testing new integer comparison analyses.
class OffsetRepresentation {
  
public:
  OffsetRepresentation();
  
  /// \brief Builds \p pointer's offset using \p base 
  OffsetRepresentation(Value* Pointer, Value* Base);
  
  /// \brief Destructor 
  virtual ~OffsetRepresentation() =0;
  
  /// \brief Returns a copy of the represented offset
  virtual OffsetRepresentation* copy() =0;
  
  /// \brief Adds two offsets of the respective representation
  virtual OffsetRepresentation* add(OffsetRepresentation* Other) =0;
  
  /// \brief Answers true if two offsets are disjoints
  virtual bool disjoint(OffsetRepresentation* Other) =0;
  
  /// \brief Narrows the offset of the respective representation
  virtual OffsetRepresentation* narrow(CmpInst::Predicate Cmp, 
    OffsetRepresentation* Other) =0;
  
  /// \brief Widens the offset of the respective representation, Before and 
  ///   After are given so its possible to calculate direction of growth.
  virtual OffsetRepresentation* widen(OffsetRepresentation* Before,
    OffsetRepresentation* After) =0;
  
  /// \brief Prints the offset representation
  void print() { }
  
};

/// \brief Class that encapsulates the concept of a pointer's offset.
/// An offset is a set of offset representations, each with it's own 
/// capabilities and operations set.
/// Use it for testing new integer comparison analyses.
class Offset{

public:
  //Functions someone should alter to add a new offset representation
  //===--------------------------------------------------------------------===//
  
  /// \brief Add custom offset representation to reps for use in obaa.
  /// This constructor should return a neutral offset element
  Offset() {
    // reps[ID] = new YourOffsetRepresentation();
  }
  
  /// \brief Creates the offset occording to \p a = \p b + offset
  Offset(const Value* A, const Value* B) {
    // reps[ID] = new YourOffsetRepresentation(a, b);
  }
  
  /// \brief Add custom offset representation required analyses
  static void getAnalysisUsage(AnalysisUsage &AU) {
    // AU.addRequired<RequiredAnalysis>();
  }
  
  //Functions that should be left alone on creating new offset representation
  //===--------------------------------------------------------------------===//
  
  /// \brief Copy contructor 
  Offset(const Offset& Other);
  
  /// \brief Destructor 
  ~Offset();
  
  /// \brief Assignment operator that provides a deep copy
  Offset& operator=(const Offset& Other);
   
  /// \brief Adds two offsets
  Offset operator+(const Offset& Other) const;
  
  /// \brief Answers true if two offsets are disjoints
  bool operator!=(const Offset& Other) const;
  
  /// \brief Narrows the offset, Base is nacessary since you only use addresses
  /// with the same base for the narrowing process. 
  void narrow(const NarrowingOp& Narrowing_op, OffsetPointer* Base);
  
  /// \brief Widens the offset
  void widen(const WideningOp& Widening_op);
  
  /// \brief Prints the offset
  void print() const;
  
private:
  std::map<const int, OffsetRepresentation*> reps;
};

}

#endif
