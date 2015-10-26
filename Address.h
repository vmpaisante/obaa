//===--------------------- Address.h - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the Address class.
///
//===----------------------------------------------------------------------===//
#ifndef __ADDRESS_H__
#define __ADDRESS_H__

// local includes
#include "Offset.h"
#include "Narrowing.h"
// llvm's includes
// c++ includes
#include <deque>
#include <map>
#include <set>

namespace llvm {

/// Forward declarations
class RangedPointer;
class Vaulue;

/// Representation of a possible pointer address. It is composed,
/// essencially, of a base pointer and an offset
class Address {
public:
  // Contructors and destructors
  Address(RangedPointer *a, RangedPointer *b, Offset o);
  Address(Address *);
  ~Address();
  // Functions that provide the object's information
  RangedPointer *getBase();
  RangedPointer *getAddressee();
  Offset getOffset();
  bool wasWidened();
  // Functions that set the object's information
  void setBase(RangedPointer *);
  void setOffset(Offset);
  // Function tha prints the object's informtation
  void print();
  // Function that expands an address, this is the most important feature
  //  of this class.
  void Expand(std::deque<Address *> &ad, std::set<Address *> &fn);
  // Functions that give the object narrowing and widening operators
  bool associateNarrowingOp(const Value *, NarrowingOp &);
  bool associateWideningOp(const Value *, WideningOp &);

private:
  // Basic contents of an address
  RangedPointer *base;
  RangedPointer *addressee;
  Offset offset;
  // Structures that hold the narrowing and widening operators
  std::map<const Value *, const NarrowingOp> narrowingOps;
  std::map<const Value *, const WideningOp> wideningOps;
  // Holds wether this address has been widened
  bool widened;
  // Holds wether the base is an argument or there is an argument on the path
  bool argument;
  // Auxilliary map for the expand function
  std::map<RangedPointer *, Offset> expanded;
};
}

#endif
