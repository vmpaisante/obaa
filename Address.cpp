//===------------------- Address.cpp - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the Address class.
///
//===----------------------------------------------------------------------===//

#include "Address.h"

using namespace llvm;

// Contructors and destructors
Address::Address(RangedPointer *a, RangedPointer *b, Offset o) {
  addressee = a;
  base = b;
  offset = o;
  widened = false;
  addressee->addresses.insert(this);
  base->bases.insert(this);

  if (isa<const Argument>(*(base->getPointer())))
    argument = true;
  else
    argument = false;
}

Address::Address(Address *copy) {
  base = copy->base;
  addressee = copy->addressee;
  offset = copy->offset;
  expanded = copy->expanded;
  widened = copy->widened;
  argument = copy->argument;
  narrowingOps = copy->narrowingOps;
  wideningOps = copy->wideningOps;

  addressee->addresses.insert(this);
  base->bases.insert(this);
}

Address::~Address() {
  addressee->addresses.erase(this);
  base->bases.erase(this);
}

// Functions that provide the object's information
RangedPointer *Address::getBase() { return base; }

RangedPointer *Address::getAddressee() { return addressee; }

Offset Address::getOffset() { return offset; }

bool Address::wasWidened() { return widened; }

// Functions that set the object's information
void Address::setBase(RangedPointer *b) { base = b; }

void Address::setOffset(Offset o) { offset = o; }

// Function tha prints the object's informtation
void Address::print() {
  if (base->getPointer()->getValueName() == NULL)
    errs() << *(base->getPointer());
  else
    errs() << base->getPointer()->getName();

  errs() << " + ";
  offset.print();
}

// Function that expands an address, this is the most important feature
//  of this class.
void Address::Expand(std::deque<Address *> &adq, std::set<Address *> &fn) {
  // Was the base already expanded, if no expansion can occur.
  //  If it was expanded, a new widening op must be created
  if (expanded.find(base) == expanded.end()) {
    std::set<Address *> aux = base->addresses;
    for (auto i = aux.begin(), ie = aux.end(); i != ie; i++) {
      // For each address from base, a new address must be formed
      //  combining each with this address
      Address *ad = (*i);
      Offset r = ad->offset + offset;
      Address *newAddress = new Address(addressee, ad->base, r);

      // If there is an argument flag in any of the addresses,
      // then the new address will have the argument flag
      if (argument)
        newAddress->argument = true;
      else if (ad->argument)
        newAddress->argument = true;

      // The expanded set of the new address will have all the pointers
      // in the expanded of this address plus it's base
      newAddress->expanded = expanded;
      newAddress->expanded[base] = offset;

      // If there is a widened flag in any of the addresses,
      // then the new address will have the widened flag
      if (widened)
        newAddress->widened = true;
      else if (ad->widened)
        newAddress->widened = true;

      // The new address will have the same widening operators
      //  and contextualized narrowing operators.
      //  Possible bug: it only inserts the first narrowing operator
      //  found on the path from the same base pointer,
      //  I suspect it is the most restrictive, since
      //  I haven't found an example that says otherwise.
      newAddress->narrowingOps = narrowingOps;
      for (auto i : ad->narrowingOps)
        if (newAddress->narrowingOps.find(i.first) ==
            newAddress->narrowingOps.end())
          newAddress->narrowingOps[i.first] = i.second.contextualize(offset);

      newAddress->wideningOps = wideningOps;
      for (auto i : ad->wideningOps)
        if (newAddress->wideningOps.find(i.first) ==
            newAddress->wideningOps.end())
          newAddress->wideningOps[i.first] = i.second;

      adq.push_front(newAddress);
    }
  } else {
    // New widening operator
    WideningOp r;
    r.before = expanded.at(base);
    r.after = offset;
    wideningOps[base->getPointer()] = r;
    for (auto i : adq) {
      i->widened = true;
      // Adds the address's narrowing operators to all others
      for (auto j : narrowingOps)
        if (i->narrowingOps.find(j.first) == i->narrowingOps.end())
          i->narrowingOps[j.first] = j.second;
      // Adds the address's narrowing operators to all others
      for (auto j : wideningOps)
        i->wideningOps[j.first] = j.second;
      // Adds the address's expanded adresses to all others
      for (auto j : expanded)
        i->expanded.insert(j);
    }
    for (auto i : fn) {
      i->widened = true;
      // Adds the address's narrowing operators to all others
      for (auto j : narrowingOps)
        if (i->narrowingOps.find(j.first) == i->narrowingOps.end())
          i->narrowingOps[j.first] = j.second;
      // Adds the address's narrowing operators to all others
      for (auto j : wideningOps)
        i->wideningOps[j.first] = j.second;
      // Adds the address's expanded adresses to all others
      for (auto j : expanded)
        i->expanded.insert(j);
    }
  }
  delete this;
}

// Functions that give the object narrowing and widening operators
bool Address::associateNarrowingOp(const Value *v, NarrowingOp &no) {
  narrowingOps[v] = no;
  return true;
}

bool Address::associateWideningOp(const Value *v, WideningOp &wo) {
  wideningOps[v] = wo;
  return true;
}
