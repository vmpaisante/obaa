//===------------------- Address.cpp - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief
///
//===----------------------------------------------------------------------===//

#include "Address.h"

using namespace llvm;

Address::Address(const OffsetPointer *A, const OffsetPointer *B, 
  const Offset& O) {

  addressee = A;
  base = B;
  offset = O;
  
  widened = false;
  if(isa<const Argument>(*(B->getPointer())))
    argument = true;
  else argument = false;
  if(isa<const GlobalVariable>(*(B->getPointer())))
    global = true;
  else global = false;
  
}
