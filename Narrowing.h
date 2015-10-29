//===------------------- Narrowing.h - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This is a simple interface dedicated to hold information about narrowing
/// and widening operations for obaa. Since these operations are executed
/// last in obaa, these helper structures are necessary for storage.
///
//===----------------------------------------------------------------------===//
#ifndef __NARROWING_H__
#define __NARROWING_H__

// local includes
#include "Offset.h"
// llvm's includes
#include "llvm/IR/InstrTypes.h"
// libc includes
#include <set>

namespace llvm {

// Forward declarations
class Value;
class PHINode;

/// \brief struct that holds comparisons in the program. It is useful 
/// to create in obaa the narrowing operators.
struct NarrowingData {
  const CmpInst::Predicate cmpOp;
  const Value *cmpV1;
  const Value *cmpV2;
  std::set<const PHINode *> sigmas;
  
  NarrowingData(const CmpInst::Predicate op, const Value* v1, const Value* v2) 
    : cmpOp(op), cmpV1(v1), cmpV2(v2) { }
};

/// \brief struct that hold information about a narrowing operation to be 
/// performed by obaa
struct NarrowingOp {
  const CmpInst::Predicate cmpOp;
  const Value *cmpV;
  const Offset context;

  /// \brief constructor that doesn't epecify any context, for using early
  NarrowingOp(const CmpInst::Predicate op, const Value* v) 
    : cmpOp(op), cmpV(v) { }
  /// \brief Constructor that specify context, for using on address expansion
  NarrowingOp(const CmpInst::Predicate op, const Value* v, const Offset c) 
    : cmpOp(op), cmpV(v), context(c) { }
  /// \brief Simple function that returns a contextualized version of the 
  /// operator in address expansion
  NarrowingOp contextualize(const Offset c) const;
};

/// \brief struct that hold information about a widening operation to be 
/// performed by obaa
struct WideningOp {
  const Offset before;
  const Offset after;
  
  WideningOp(const Offset b, const Offset a) : before(b), after(a) { }
};
}

#endif
