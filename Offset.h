//===---------------------- Offset.h - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the Offset class and
/// offset representations.
///
//===----------------------------------------------------------------------===//

#ifndef __OFFSET_H__
#define __OFFSET_H__

#include <llvm/IR/InstrTypes.h>
#include <map>

namespace llvm {

// Forward declarations
class AnalysisUsage;
class NarrowingOp;
class WideningOp;
class Value;

// Abstract class for implementing offset representations.
//  Use it for testing new integer comparison analyses.
class OffsetRepresentation{
  public:
  
  OffsetRepresentation();
  OffsetRepresentation(Value* v);
  // Add two offsets of the respective representation
  virtual OffsetRepresentation* add(OffsetRepresentation* Other) =0;
  // Answers true if two offsets are disjoints
  virtual bool disjoint(OffsetRepresentation* Other) =0;
  // Narrows the offset of the respective representation
  virtual OffsetRepresentation* narrow(OffsetRepresentation* Other, 
    CmpInst::Predicate cmp) =0;
  // Widens the offset of the respective representation
  virtual OffsetRepresentation* widen(OffsetRepresentation* Other,
    bool negative, bool positive) =0;
  
};

class Offset{
  public:
  
  //Add custom offset representation to reps for use in obaa 
  Offset()
  {
    //reps[ID] = new YourOffsetRepresentation();
  }
  Offset(const Value* v)
  {
    //reps[ID] = new YourOffsetRepresentation(v);
  }
  //Add custom offset representation required analyses
  static void getAnalysisUsage(AnalysisUsage &AU)
  {
  
  }
  //Add custom offset representation init function
  static void initRepresentations()
  {
    
  }
  // Add two offsets
  Offset operator+(const Offset& Other)
  {
    Offset result;
    for(auto i : result.reps)
    {
      int ID = i.first;
      result.reps[ID] = reps[ID]->add(Other.reps.at(ID));
    }
    return result;
    
  }
  // Answers true if two offsets are disjoints
  bool operator!=(const Offset& Other)
  {
    for(auto i : reps)
    {
      int ID = i.first;
      if(reps[ID]->disjoint(Other.reps.at(ID)))
        return true;
    }
    return false;
  }
  // Narrows the offset
  void narrow(const NarrowingOp& narrowing_op)
  {
  
  }
  // Widens the offset
  void widen(const WideningOp& widening_op)
  {
  
  }
  void print()
  {
  
  }
  
  private:
  
  std::map<int, OffsetRepresentation*> reps;
};

}

#endif
