//===------------------- Offset.cpp - Pass definition -----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief
///
//===----------------------------------------------------------------------===//

#include "Offset.h"

using namespace llvm;

/// \brief Copy contructor 
Offset::Offset(const Offset& Other) {
  for (auto i : Other.reps) {
    reps[i.first] = i.second->copy();
  }
}

/// \brief Destructor 
Offset::~Offset() {
  for (auto i : reps) delete i.second;
}

/// \brief Assignment operator that provides a deep copy
Offset& Offset::operator=(const Offset& Other) {
  for (auto i : reps) delete i.second;
  for (auto i : Other.reps) {
    reps[i.first] = i.second->copy();
  }
  return *this;
}

/// \brief Adds two offsets
Offset Offset::operator+(const Offset& Other) const {
  Offset result;
  for (auto i : result.reps) {
    const int ID = i.first;
    result.reps[ID] = reps.at(ID)->add(Other.reps.at(ID));
  }
  return result;
  
}

/// \brief Answers true if two offsets are disjoints
bool Offset::operator!=(const Offset& Other) const {
  for (auto i : reps) {
    const int ID = i.first;
    if(reps.at(ID)->disjoint(Other.reps.at(ID))) return true;
  }
  return false;
}

/// \brief Narrows the offset
void Offset::narrow(const NarrowingOp& narrowing_op) {
  for (auto i : reps) {
    const int ID = i.first;
    
  }
}

/// \brief Widens the offset
void Offset::widen(const WideningOp& widening_op) { }

/// \brief Prints the offset
void Offset::print() const { 
  bool notFirst = false;
  errs() << "(";
  for (auto i : reps) {
    if(notFirst) errs() << " & ";
    else notFirst = true;
    i.second->print();
  }
  errs() << ")";
}
