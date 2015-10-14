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
#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

#include <vector>

namespace llvm {

/// Forward declarations
class Type;

/// Representation of types as sequences of primitive values
class Primitives
{
  public:
  //Holds a Primitive Layout for a determined Type
  struct PrimitiveLayout
  {
    Type * type;
    std::vector<int> layout;
    PrimitiveLayout(Type* ty, std::vector<int> lay)
    {
      type = ty;
      layout = lay;
    }
  };
  struct NumPrimitive
  {
    Type * type;
    int num;
    NumPrimitive(Type* ty, int n)
    {
      type = ty;
      num = n;
    }
  };
  std::vector<PrimitiveLayout*> PrimitiveLayouts;
  std::vector<NumPrimitive*> NumPrimitives;
  
  std::vector<int> getPrimitiveLayout(Type* type);
  int getNumPrimitives(Type* type);
  llvm::Type* getTypeInside(Type* type, int i);
  int getSumBehind(std::vector<int> v, unsigned int i);
};

}

#endif
