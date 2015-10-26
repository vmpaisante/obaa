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

#include "RangedPointer.h"
#include "Address.h"
#include "OffsetBasedAliasAnalysis.h"

using namespace llvm;

// Contructors and destructors
RangedPointer::RangedPointer(const Value *p) { pointer = p; }
RangedPointer::RangedPointer(const Value *p, PointerTypes pt) {
  pointer = p;
  pointerType = pt;
}
RangedPointer::RangedPointer(RangedPointer *) {}

// Functions that provide the object's information
const Value *RangedPointer::getPointer() { return pointer; }
enum RangedPointer::PointerTypes RangedPointer::getPointerType() {
  return pointerType;
}
std::set<Address *>::iterator RangedPointer::addr_begin() {
  return addresses.begin();
}
std::set<Address *>::iterator RangedPointer::addr_end() {
  return addresses.end();
}
bool RangedPointer::addr_empty() { return addresses.empty(); }
std::set<Address *>::iterator RangedPointer::bases_begin() {
  return bases.begin();
}
std::set<Address *>::iterator RangedPointer::bases_end() { return bases.end(); }

// Functions that set the object's information
void RangedPointer::setPointerType(PointerTypes pt) { pointerType = pt; }

// Function tha prints the object's information
void RangedPointer::print() {}

// Function that finds the pointer's possible addresses,
//  this is the most important feature of this class.
void RangedPointer::processInitialAddresses(OffsetBasedAliasAnalysis* analysis){
  // Global variables are pointers by standart
  if(isa<const GlobalVariable>(*pointer)) { pointerType = Alloc; }
  // The argv argument has it's own allocation, for the rest is 
  //  necessary check what parameters are beeing sent in function
  //  calls
  else if(const Argument* p = dyn_cast<Argument>(pointer)) {
    if(p->getName().equals("argv")) { pointerType = Alloc; }
    else {
      pointerType = Phi;
      const Function* F = p->getParent();
      for(auto ui = F->user_begin(), ue = F->user_end(); ui != ue; ui++) {
        const User* u = *ui;
        if(const CallInst* caller = dyn_cast<CallInst>(u)) {
          int anum = caller->getNumArgOperands();
          int ano = p->getArgNo();
          if(ano <= anum) {
            /// create address
            RangedPointer* Base = analysis->getRangedPointer
              (caller->getArgOperand(ano));
            new Address(this, Base, Offset(Base->getPointer(), getPointer()) );
          }
          else {
            /// TODO: support standard values in cases where the argument
            /// has a standard value and does not appear in function call
            errs() << "!: ERROR (Not enough arguments):\n";
            errs() << *p << " " << ano << "\n";
            errs() << *u << "\n";
            pointerType = Unk;
          }
        }
      }
    }
  }
  // An alloca instruction is an allocation
  else if(isa<const AllocaInst>(*pointer)) { pointerType = Alloc; }
  
  else if(const CallInst* p = dyn_cast<CallInst>(pointer)) {
    Function* CF = p->getCalledFunction();
    if(CF) {
      if( strcmp( CF->getName().data(), "malloc") == 0
      or strcmp( CF->getName().data(), "calloc") == 0  ) { 
        //if is an allocation function
        pointerType = Alloc;
      }
      else if(strcmp( CF->getName().data(), "realloc") == 0) {
        /// realloc is of the same name as it's first argument
        pointerType = Cont;
        const Value* BasePtr = p->getOperand(0);
        RangedPointer* Base = analysis->getRangedPointer(BasePtr);
        new Address(this, Base, Offset(Base->getPointer(), getPointer()) );
      }
      else { //else, addresses must be calculated
        //errs() << "Function Call.\n";
        pointerType = Phi;
        for (auto i = inst_begin(CF), e = inst_end(CF); i != e; i++)
          if(isa<const ReturnInst>(*i)) {
            /// create address
            const Value* RetPtr = ((ReturnInst*)&(*i))->getReturnValue();
            RangedPointer* Base = analysis->getRangedPointer(RetPtr);
            new Address(this, Base, Offset(Base->getPointer(), getPointer()) );
          }
        if(addresses.empty()) pointerType = Unk;
      }
    }
    else { pointerType = Unk; }
  }
  else if(const BitCastInst* p = dyn_cast<BitCastInst>(pointer)) {
    pointerType = Cont;
    /// create address
    Value* BasePtr = p->getOperand(0);
    RangedPointer* Base = analysis->getRangedPointer(BasePtr);
    new Address(this, Base, Offset(Base->getPointer(), getPointer()) );
  }
  else if(isa<const LoadInst>(*pointer)) { pointerType = Unk; }
  else if(const PHINode* p = dyn_cast<PHINode>(pointer)) {
    pointerType = Phi;
    // create addresses
    unsigned int num = p->getNumIncomingValues();
    for (unsigned int i = 0; i < num; i++)
    {
      Value* BasePtr = p->getIncomingValue(i);
      RangedPointer* Base = analysis->getRangedPointer(BasePtr);
      new Address(this, Base, Offset(Base->getPointer(), getPointer()) );
    }  
  }
  else if(const GetElementPtrInst* p = dyn_cast<GetElementPtrInst>(pointer)) {
    pointerType = Cont;
    const Value* BasePtr = p->getPointerOperand();
    RangedPointer* Base = analysis->getRangedPointer(BasePtr);
    new Address(this, Base, Offset(Base->getPointer(), getPointer()) );
  }
  else if(const GEPOperator* p = dyn_cast<GEPOperator>(pointer)) {
    pointerType = Cont;
    const Value* BasePtr = p->getPointerOperand();
    RangedPointer* Base = analysis->getRangedPointer(BasePtr);
    new Address(this, Base, Offset(Base->getPointer(), getPointer()) );
  }
  else if(isa<const ConstantPointerNull>(*pointer)) { pointerType = Null; }
  else if(const ConstantExpr* p = dyn_cast<ConstantExpr>(pointer)) {
    const char* operation = p->getOpcodeName();
    if(strcmp(operation, "bitcast") == 0)
    {
      pointerType = Cont;
      /// create address
      Value* BasePtr = p->getOperand(0);
      RangedPointer* Base = analysis->getRangedPointer(BasePtr);
      new Address(this, Base, Offset(Base->getPointer(), getPointer()) );
    }
    else
    {
      pointerType = Unk;
      errs() << *p << " -> Unknown ConstantExpr\n";
    }
  }
  else if(isa<const Function>(*pointer)) { pointerType = Alloc; }
  else {
    pointerType = Unk;
    errs() << *pointer << " -> Unknown Value\n";
  }  
}

// function for the path sensitive analysis
void RangedPointer::getUniquePath() {}
