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

#include "Offset.h"

#include "llvm/Pass.h"
#include "llvm/IR/InstrTypes.h"

#include <set>

namespace llvm {

/// Forward declarations
class Value;
class PHINode;

/// classes that help implementing the narrowing operations
class NarrowingData
{
  public:
  CmpInst::Predicate cmp_op;
  Value* cmp_v1;
  Value* cmp_v2;
  std::set<const PHINode*> sigmas;
};

class NarrowingOp
{
  public:
  CmpInst::Predicate cmp_op;
  const Value* cmp_v;
  Offset context;
  
  NarrowingOp* contextualize(Offset c);
};

class WideningOp
{
  public:
  bool positive;
  bool negative;
  
};

}

#endif

