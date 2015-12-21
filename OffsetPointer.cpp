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
#include "OffsetBasedAliasAnalysis.h"
#include "Offset.h"
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
  // Global variables in LLVM are pointers by definition with their own alloc
  if(isa<const GlobalVariable>(*pointer)) { pointer_type = Alloc; }
  // TODO: STOP
  else if(const Argument* p = dyn_cast<Argument>(pointer)) { 
    if(p->getName().equals("argv")) { pointer_type = Alloc; }
    else {
      pointer_type = Phi;
      const Function* F = p->getParent();
      for(auto ui = F->user_begin(), ue = F->user_end(); ui != ue; ui++) {
        const User* u = *ui;
        if(const CallInst* caller = dyn_cast<CallInst>(u)) {
          int anum = caller->getNumArgOperands();
          int ano = p->getArgNo();
          if(ano <= anum) {
            OffsetPointer* base = Analysis->getOffsetPointer
              (caller->getArgOperand(ano));
            if(base != NULL) 
            	new Address(this, base, Offset());
          }
          else {
            /// TODO: support standard values in cases where the argument
            /// has a standard value and does not appear in function call
            errs() << "!: ERROR (Not enough arguments):\n";
            errs() << *p << " " << ano << "\n";
            errs() << *u << "\n";
            pointer_type = Unk;
          }
        }
      }
    } 
  }
  else if(isa<const AllocaInst>(*pointer)) { 
  
  }
  else if(const CallInst* p = dyn_cast<CallInst>(pointer)) { 
  
  }
  else if(const BitCastInst* p = dyn_cast<BitCastInst>(pointer)) { 
  
  }
  else if(isa<const LoadInst>(*pointer)) { 
  
  }
  else if(const PHINode* p = dyn_cast<PHINode>(pointer)) { 
  
  }
  else if(const GetElementPtrInst* p = dyn_cast<GetElementPtrInst>(pointer)) { 
  
  }
  else if(const GEPOperator* p = dyn_cast<GEPOperator>(pointer)) { 
  
  }
  else if(isa<const ConstantPointerNull>(*pointer)) { 
  
  }
  else if(const ConstantExpr* p = dyn_cast<ConstantExpr>(pointer)) { 
  
  }
  else if(isa<const Function>(*pointer)) { 
  
  }
  else { 
    pointer_type = Unk; 
  }
}

/// \brief Function that finds the pointer's path to the root of it's local
/// tree.
/// TODO: Optimize function to take advantage of topological ordering.
void OffsetPointer::getPathToRoot() {
  OffsetPointer* current = this;
  int index = 0;
  Offset offset;
  while(true) {
    path_to_root[current] = std::pair<int, Offset>(index, offset);
    if(current->addresses.size() == 1) {
      Address* addr = *(current->addresses.begin());
      current = addr->getBase();
      if(path_to_root.count(current)) {
        //This means that the local tree is actually a lonely loop
      	// so the local tree's root will be the pointer with the highest address
      	OffsetPointer* root = NULL;
      	for(auto i : path_to_root){
      		if(root < i.first) root = i.first;
      	}
      	local_root = root;
      	break;
      }
    }
  }
}








