//===--------------- RangedPointer.h - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief
///
//===----------------------------------------------------------------------===//

#include "RangedPointer.h"
#include "Address.h"

using namespace llvm;

// Contructors and destructors
RangedPointer::RangedPointer(const Value *p) { pointer = p; }
RangedPointer::RangedPointer(const Value *p, PointerTypes pt) {
  pointer = p;
  pointerType = pt;
}
RangedPointer::RangedPointer(RangedPointer *) {}

// Functions that provide the object's information
const Value *RangedPointer::getPointer() { return pointer; }
enum RangedPointer::PointerTypes RangedPointer::getPointerType() {
  return pointerType;
}
std::set<Address *>::iterator RangedPointer::addr_begin() {
  return addresses.begin();
}
std::set<Address *>::iterator RangedPointer::addr_end() {
  return addresses.end();
}
bool RangedPointer::addr_empty() { return addresses.empty(); }
std::set<Address *>::iterator RangedPointer::bases_begin() {
  return bases.begin();
}
std::set<Address *>::iterator RangedPointer::bases_end() { return bases.end(); }

// Functions that set the object's information
void RangedPointer::setPointerType(PointerTypes pt) { pointerType = pt; }

// Function tha prints the object's information
void RangedPointer::print() {}

// Function that finds the pointer's possible addresses,
//  this is the most important feature of this class.
void processInitialAddresses(OffsetBasedAliasAnalysis *) {}

// function for the path sensitive analysis
void RangedPointer::getUniquePath() {}
