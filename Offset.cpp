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
#include "Narrowing.h"
#include "OffsetPointer.h"
#include "Address.h"

using namespace llvm;

//Functions someone should alter to add a new offset representation
//===--------------------------------------------------------------------===//
  
/// \brief Add custom offset representation to reps for use in obaa.
/// This constructor should return a neutral offset element
Offset::Offset() {
  // reps[ID] = new YourOffsetRepresentation();
}
  
/// \brief Creates the offset occording to \p a = \p b + offset
Offset::Offset(const Value* A, const Value* B) {
  // reps[ID] = new YourOffsetRepresentation(a, b);
}
  
/// \brief Add custom offset representation required analyses
void Offset::getAnalysisUsage(AnalysisUsage &AU) {
  // AU.addRequired<RequiredAnalysis>();
}
  
//Functions that should be left alone on creating new offset representation
//===--------------------------------------------------------------------===//

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
void Offset::narrow(const NarrowingOp& Narrowing_op, OffsetPointer* Base) {
  for(auto ad : Narrowing_op.cmp_v->addresses) {
    if(ad->getBase() == Base) {
      Offset narrowing_offset = ad->getOffset() + Narrowing_op.context;
      for (auto i : reps) {
        const int ID = i.first;
        reps[ID] = i.second->narrow(Narrowing_op.cmp_op, 
          narrowing_offset.reps.at(ID));
      }   
    }
  }
}

/// \brief Widens the offset
void Offset::widen(const WideningOp& Widening_op) { 
  for (auto i : reps) {
    const int ID = i.first;
    reps[ID] = i.second->widen(Widening_op.before.reps.at(ID), 
      Widening_op.after.reps.at(ID));
  }
}

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

/// \brief Prints the offset to a file
void Offset::print(raw_fd_ostream& fs) const { 
  bool notFirst = false;
  fs << "(";
  for (auto i : reps) {
    if(notFirst) fs << " & ";
    else notFirst = true;
    i.second->print(fs);
  }
  fs << ")";
}
