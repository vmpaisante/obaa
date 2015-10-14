//===------------------- Address.cpp - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the Address class.
///
//===----------------------------------------------------------------------===//

#include "Address.h"
#include "RangedPointer.h"
#include "Offset.h"

using namespace llvm;

RangedPointer* Address::getBase(){}
RangedPointer* Address::getAddressee(){}
Offset Address::getOffset(){}
bool Address::wasWidened(){}
void Address::setBase(RangedPointer*){}
void Address::print(){}

bool Address::associateNarrowingOp(const Value* v, NarrowingOp* no)
{
  Narrowing_Ops[v] = no;
  return true;
}
