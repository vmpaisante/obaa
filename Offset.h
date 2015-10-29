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
// libc includes
#include <map>

namespace llvm {

// Forward declarations
class AnalysisUsage;
class NarrowingOp;
class WideningOp;
class Value;

/// \brief Abstract class for implementing offset representations.
/// Use it for testing new integer comparison analyses.
class OffsetRepresentation {
  
public:
  OffsetRepresentation();
  
  /// \brief Builds \p pointer's offset using \p base 
  OffsetRepresentation(Value* base, Value* pointer);
  
  /// \brief Destructor 
  virtual ~OffsetRepresentation() =0;
  
  /// \brief Returns a copy of the represented offset
  virtual OffsetRepresentation* copy() =0;
  
  /// \brief Adds two offsets of the respective representation
  virtual OffsetRepresentation* add(OffsetRepresentation* Other) =0;
  
  /// \brief Answers true if two offsets are disjoints
  virtual bool disjoint(OffsetRepresentation* Other) =0;
  
  /// \brief Narrows the offset of the respective representation
  virtual OffsetRepresentation* narrow(OffsetRepresentation* Other, 
    CmpInst::Predicate cmp) =0;
  
  /// \brief Widens the offset of the respective representation
  virtual OffsetRepresentation* widen(OffsetRepresentation* Other,
    bool negative, bool positive) =0;
  
  /// \brief Prints the offset representation
  void print() { }
  
};

/// \brief Class that encapsulates the concept of a pointer's offset.
/// An offset is a set of offset representations, each with it's own 
/// capabilities and operations set.
/// Use it for testing new integer comparison analyses.
class Offset{

public:
  /// \brief Add custom offset representation to reps for use in obaa 
  Offset() {
    // reps[ID] = new YourOffsetRepresentation();
  }
  
  /// \brief Creates the offset occording to \p a = \p b + offset
  Offset(const Value* b, const Value* a) {
    // reps[ID] = new YourOffsetRepresentation(b, a);
  }
  
  /// \brief Copy contructor 
  Offset(const Offset& Other) {
    for (auto i : Other.reps) {
      reps[i.first] = i.second->copy();
    }
  }
  
  /// \brief Destructor 
  ~Offset() {
    for (auto i : reps) delete i.second;
  }
  
  /// \brief Add custom offset representation required analyses
  static void getAnalysisUsage(AnalysisUsage &AU) {
    // AU.addRequired<RequiredAnalysis>();
  }
  
  /// \brief Add custom offset representation init function
  static void initRepresentations() { }
  
  /// \brief Assignment operator that provides a deep copy
  Offset& operator=(const Offset& Other) {
    for (auto i : reps) delete i.second;
    for (auto i : Other.reps) {
      reps[i.first] = i.second->copy();
    }
    return *this;
  }
  
  /// \brief Adds two offsets
  Offset operator+(const Offset& Other) const {
    Offset result;
    for (auto i : result.reps) {
      const int ID = i.first;
      result.reps[ID] = reps.at(ID)->add(Other.reps.at(ID));
    }
    return result;
    
  }
  
  /// \brief Answers true if two offsets are disjoints
  bool operator!=(const Offset& Other) const {
    for (auto i : reps) {
      const int ID = i.first;
      if(reps.at(ID)->disjoint(Other.reps.at(ID))) return true;
    }
    return false;
  }
  
  /// \brief Narrows the offset
  void narrow(const NarrowingOp& narrowing_op) { }
  
  /// \brief Widens the offset
  void widen(const WideningOp& widening_op) { }
  
  /// \brief Prints the offset
  void print() const { }
  
private:
  std::map<const int, OffsetRepresentation*> reps;
};

}

#endif
