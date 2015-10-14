//===--------------- RangedPointer.h - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief 
///
//===----------------------------------------------------------------------===//
#ifndef __RANGED_POINTER_H__
#define __RANGED_POINTER_H__

#include <set>

namespace llvm {

/// Forward declarations
class Value;
class Address;
class Offset;
class Value;
class Use;
class OffsetBasedAliasAnalysis;

/// \brief Pointer representation in which each pointer is a set of
/// possible addresses
class RangedPointer
{
  friend class Address;
  
  public:
  enum PointerTypes
  {
    Unk = 0,
    Alloc = 1,
    Phi = 2,
    Cont = 3,
    Null = 4
  };
  
  RangedPointer(const Value* pointer);
  const Value* getPointer();
  enum PointerTypes getPointerType();
  std::set<Address*>::iterator addr_begin();
  std::set<Address*>::iterator addr_end();
  bool addr_empty();
  std::set<Address*>::iterator bases_begin();
  std::set<Address*>::iterator bases_end();
  
  void processInitialAddresses(OffsetBasedAliasAnalysis* analysis);
  
  void setPointerType(PointerTypes);
  
  void print();
  
  private:  
  const Value* Pointer;
  std::set<Address*> Addresses;
  std::set<Address*> Bases;
  PointerTypes PointerType;

};

}

#endif
