//===------------------- Narrowing.h - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief
///
//===----------------------------------------------------------------------===//
#ifndef __NARROWING_H__
#define __NARROWING_H__

// Project's includes
#include "Offset.h"
// llvm's includes
#include "llvm/IR/Value.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
// libc includes
#include <set>
#include <map>
#include <deque>

namespace llvm {

/// classes that help implementing the narrowing operations
class NarrowingData {
public:
  CmpInst::Predicate cmp_op;
  Value *cmp_v1;
  Value *cmp_v2;
  std::set<const PHINode *> sigmas;
};

class NarrowingOp {
public:
  CmpInst::Predicate cmp_op;
  const Value *cmp_v;
  Offset context;

  NarrowingOp contextualize(Offset c);
};

class WideningOp {
public:
  Offset before;
  Offset after;
};
}

#endif
