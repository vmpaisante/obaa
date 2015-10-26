//===----------------- Narrowing.cpp - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief
///
//===----------------------------------------------------------------------===//

#include "Narrowing.h"

using namespace llvm;

NarrowingOp NarrowingOp::contextualize(Offset c) {
  NarrowingOp result;
  result.cmp_op = cmp_op;
  result.cmp_v = cmp_v;
  result.context = context + c;
  return result;
}
