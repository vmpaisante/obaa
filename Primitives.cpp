//===------------------ Primitives.h - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the Primitives class
///
//===----------------------------------------------------------------------===//

#include "Primitives.h"
#include "llvm/IR/Value.h"

using namespace llvm;

std::vector<int> Primitives::getPrimitiveLayout(Type* type){}
int Primitives::getNumPrimitives(Type* type){}
llvm::Type* Primitives::getTypeInside(Type* type, int i){}
int Primitives::getSumBehind(std::vector<int> v, unsigned int i){}
