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
#ifndef __RANGED_POINTER_H__
#define __RANGED_POINTER_H__

// Project's includes
#include "Offset.h"
// llvm's includes
#include "llvm/IR/Value.h"
#include "llvm/IR/Use.h"
// libc includes
#include <set>

namespace llvm {

/// Forward declarations
class OffsetBasedAliasAnalysis;
class Address;

/// \brief Pointer representation in which each pointer is a set of
/// possible addresses
class RangedPointer {
  friend class Address;

public:
  enum PointerTypes { Unk = 0, Alloc = 1, Phi = 2, Cont = 3, Null = 4 };

  // Contructors and destructors
  RangedPointer(const Value*);
  RangedPointer(const Value*, PointerTypes);
  RangedPointer(RangedPointer*);

  // Functions that provide the object's information
  const Value *getPointer();
  enum PointerTypes getPointerType();
  std::set<Address *>::iterator addr_begin();
  std::set<Address *>::iterator addr_end();
  bool addr_empty();
  std::set<Address *>::iterator bases_begin();
  std::set<Address *>::iterator bases_end();

  // Functions that set the object's information
  void setPointerType(PointerTypes);

  // Function tha prints the object's information
  void print();

  // Function that finds the pointer's possible addresses,
  //  this is the most important feature of this class.
  void processInitialAddresses(OffsetBasedAliasAnalysis *);

private:
  const Value *pointer;
  std::set<Address *> addresses;
  std::set<Address *> bases;
  PointerTypes pointerType;
  // members that help topological ordering and scc finding
  int color;
  int scc;
  // function and structures for the local analysis
  RangedPointer *localTree;
  std::map<RangedPointer *, std::pair<int, Offset>> path;
  void getUniquePath();
};
}

#endif
