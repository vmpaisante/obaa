//===--------------------- Address.h - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the Address class.
///
//===----------------------------------------------------------------------===//
#ifndef __ADDRESS_H__
#define __ADDRESS_H__

#include <set>
#include <map>
#include <deque>

namespace llvm {

/// Forward declarations
class RangedPointer;
class NarrowingOp;
class WideningOp;
class Value;
class Offset;


/// Representation of a possible pointer address. It is composed,
/// essencially, of a base pointer and an offset
class Address
{
  public:
  RangedPointer* getBase();
  RangedPointer* getAddressee();
  Offset getOffset();
  bool wasWidened();
  
  void setBase(RangedPointer*);
  
  void print();
  
  bool associateNarrowingOp(const Value*, NarrowingOp*);
  
  private:
  RangedPointer* base;
  RangedPointer* addressee;
  Offset* offset;
  
  std::map<const Value*, NarrowingOp*> Narrowing_Ops;  
    
  bool widened;
  
};

}

#endif
