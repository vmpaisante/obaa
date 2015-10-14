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
#include "llvm/IR/Value.h"

using namespace llvm;

RangedPointer::RangedPointer(const Value* pointer){}
const Value* RangedPointer::getPointer(){}
enum RangedPointer::PointerTypes RangedPointer::getPointerType(){}
std::set<Address*>::iterator RangedPointer::addr_begin(){}
std::set<Address*>::iterator RangedPointer::addr_end(){}
bool RangedPointer::addr_empty(){}
std::set<Address*>::iterator RangedPointer::bases_begin(){}
std::set<Address*>::iterator RangedPointer::bases_end(){}
void RangedPointer::setPointerType(PointerTypes){}
void RangedPointer::print(){}

void RangedPointer::processInitialAddresses(OffsetBasedAliasAnalysis* analysis){}
