//===--------------------- Address.h - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Address class. An address
/// is composed mainly of a base and an offset, but here, an address can hold
/// it's addressee, widening and narrowing operations that should happen and
/// some other simple data. 
///
//===----------------------------------------------------------------------===//
#ifndef __ADDRESS_H__
#define __ADDRESS_H__

// local includes
#include "Offset.h"
#include "Narrowing.h"
// c++ includes
#include <deque>
#include <map>
#include <set>

namespace llvm {

// Forward declarations
class OffsetPointer;
class Value;

/// \brief Representation of a possible pointer address. It is composed,
/// essencially, of a base pointer and an offset
class Address {
public:
  // Contructors and destructors
  Address(const OffsetPointer *A, const OffsetPointer *B, const Offset& O);
  Address(const Address& A);
  ~Address();
  // Functions that provide the object's information
  const OffsetPointer *getBase() const;
  const OffsetPointer *getAddressee() const;
  const Offset getOffset() const;
  bool wasWidened() const;
  bool hasArgFlag() const;
  bool hasGlobalFlag() const;
  /// \brief Function tha prints the object's informtation
  void print() const;
  /// \brief Function that expands an address, this is the most important 
  ///  feature of this class. TODO: do better comment
  void Expand(std::deque<Address *>& Ad, std::set<Address *>& Fn);
  // Functions that give the object narrowing and widening operators
  bool associateNarrowingOp(const Value* V, const NarrowingOp& No);
  bool associateWideningOp(const Value* V, const WideningOp& Wo);

private:
  // Basic contents of an address
  const OffsetPointer* base;
  const OffsetPointer* addressee;
  const Offset offset;
  // Structures that hold the narrowing and widening operators
  std::map<const Value *, const NarrowingOp> narrowing_ops;
  std::map<const Value *, const WideningOp> widening_ops;
  /// \brief Holds wether this address has been widened
  bool widened;
  /// \brief Holds wether the base is an argument or there is an argument on the path
  bool argument;
  /// \brief Holds wether the base is a global
  bool global;
  /// \brief Auxilliary map for the expand function
  std::map<const OffsetPointer *, const Offset> expanded;
};
}

#endif
