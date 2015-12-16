//===------------- OffsetPointer.cpp - Pass definition ----------*- C++ -*-===//
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
#include "OffsetPointer.h"
#include "Address.h"
// llvm includes
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Casting.h"
// STL includes
#include <cassert>

using namespace llvm;

/// \brief Simple constructor that recieves a simple Value* with pointer type
OffsetPointer::OffsetPointer(const Value* V) : pointer(V) { 
  //V must be a pointer
  assert(V->getType()->isPointerTy() && "Tried to build non pointer.");
  
  pointer_type = PointerTypes::Unk;  
}

/// \brief Constructor that recieves a simple Value* and a Type
OffsetPointer::OffsetPointer(const Value* V, PointerTypes Pt) : pointer(V) { 
  //V must be a pointer
  assert(V->getType()->isPointerTy() && "Tried to build non pointer.");
  
  pointer_type = Pt;  
}

/// \brief Returns the LLVM's Value to which the object represents
const Value* OffsetPointer::getPointer() const { return pointer; }

/// \brief Returns which kind of pointer in the graph this object
/// has
enum OffsetPointer::PointerTypes OffsetPointer::getPointerType() const { 
  return pointer_type; 
  
}

/// \brief Returns the initial iterator of the address set
std::set<Address *>::iterator OffsetPointer::addr_begin() const { 
  return addresses.begin();
}

/// \brief Returns the end iterator of the address set
std::set<Address *>::iterator OffsetPointer::addr_end() const { 
  return addresses.end();
}

/// \brief Returns whether the address set is empty
bool OffsetPointer::addr_empty() const { return addresses.empty(); }

/// \brief Returns the initial iterator of the bases set
std::set<Address *>::iterator OffsetPointer::bases_begin() const { 
  return bases.begin();
}

/// \brief Returns the end iterator of the bases set
std::set<Address *>::iterator OffsetPointer::bases_end() const { 
  return bases.end();
}

/// \brief Sets which kind of pointer in the graph this object has
void OffsetPointer::setPointerType(OffsetPointer::PointerTypes Pt) { 
  pointer_type = Pt; 
}

/// \brief Function tha prints the object's information
void OffsetPointer::print() const {
  errs() << "Pointer: " << *pointer << "\n";
  errs() << "Type: ";
  if(pointer_type == OffsetPointer::Unk)
    errs() << "Unknown\n";
  else if(pointer_type == OffsetPointer::Alloc)
    errs() << "Allocation\n";
  else if(pointer_type == OffsetPointer::Phi)
    errs() << "Phi\n";
  else if(pointer_type == OffsetPointer::Cont)
    errs() << "Continuous\n";
  else if(pointer_type == OffsetPointer::Null)
    errs() << "Null\n";
  
  errs() << "{";
  bool notFirst = false;
  for(auto i : addresses)
  {
    if(notFirst) errs() << "; ";
    i->print();
  }
  errs() << "}\n";
}

/// \brief Function that finds the pointer's possible addresses,
void OffsetPointer::processInitialAddresses(OffsetBasedAliasAnalysis* Analysis){
  if(isa<const GlobalVariable>(*pointer)) { }
  else if(const Argument* p = dyn_cast<Argument>(pointer)) { }
  else if(isa<const AllocaInst>(*pointer)) { }
  else if(const CallInst* p = dyn_cast<CallInst>(pointer)) { }
  else if(const BitCastInst* p = dyn_cast<BitCastInst>(pointer)) { }
  else if(isa<const LoadInst>(*pointer)) { }
  else if(const PHINode* p = dyn_cast<PHINode>(pointer)) { }
  else if(const GetElementPtrInst* p = dyn_cast<GetElementPtrInst>(pointer)) { }
  else if(const GEPOperator* p = dyn_cast<GEPOperator>(pointer)) { }
  else if(isa<const ConstantPointerNull>(*pointer)) { }
  else if(const ConstantExpr* p = dyn_cast<ConstantExpr>(pointer)) { }
  else if(isa<const Function>(*pointer)) { }
  else { 
    pointer_type = Unk; 
  }
}










