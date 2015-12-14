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

// local includes
#include "Address.h"
#include "OffsetPointer.h"
// llvm includes
#include "llvm/IR/Argument.h"
#include "llvm/IR/GlobalVariable.h"

using namespace llvm;

/// \brief Main constructor
Address::Address(OffsetPointer* const A, OffsetPointer* const B, 
const Offset& O) : base(B), addressee(A), offset(O) {
  widened = false;
  
  if(isa<const Argument>(*(B->getPointer()))) argument = true;
  else argument = false;
  
  if(isa<const GlobalVariable>(*(B->getPointer()))) global = true;
  else global = false;
  
  //Inserts the address edge in the graph
  addressee->addresses.insert(this);
  base->bases.insert(this);
  
}

/// \brief Copy constructor
Address::Address(const Address& A) : 
base(A.base), addressee(A.addressee), offset(A.offset) {
  narrowing_ops = A.narrowing_ops;
  widening_ops = A.widening_ops;
  widened = A.widened;
  argument = A.argument;
  global = A.global;
  
  //Inserts the address edge in the graph
  addressee->addresses.insert(this);
  base->bases.insert(this);
}

/// \brief Destructor that removes the address from the graph
Address::~Address() {
  addressee->addresses.erase(this);
  base->bases.erase(this);
}

/// \brief Returns the base of this address
OffsetPointer* Address::getBase() const { return base; }

/// \brief Returns the addressee of this address
OffsetPointer* Address::getAddressee() const { return addressee; }

/// \brief Returns the offset of this address
const Offset Address::getOffset() const { return offset; }

bool Address::wasWidened() const { return widened; }

bool Address::hasArgFlag() const { return argument; }

bool Address::hasGlobalFlag() const { return global; }


