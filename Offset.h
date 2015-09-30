//===---------------------- Offset.h - Pass definition ----------*- C++ -*-===//
//
//             Offset Based Alias Analysis for The LLVM Compiler
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the Offset class.
///
//===----------------------------------------------------------------------===//

#ifndef __OFFSET_H__
#define __OFFSET_H__

#include <llvm/IR/InstrTypes.h>
#include <set>

namespace llvm {

/// Forward declarations
class AnalysisUsage;
class NarrowingOp;
class WideningOp;

class OffsetRepresentation{
  public:
  
  virtual OffsetRepresentation* add(OffsetRepresentation* Other) =0;
  virtual OffsetRepresentation* disjoint(OffsetRepresentation* Other) =0;
  virtual OffsetRepresentation* narrow(OffsetRepresentation* Other, 
    CmpInst::Predicate cmp) =0;
  virtual OffsetRepresentation* widen(OffsetRepresentation* Other,
    bool negative, bool positive) =0;
  
};

class Offset{
  public:
  
  //Add custom offset representation to reps for use in obaa 
  Offset()
  {
    //reps.insert(new YourOffsetRepresentation());
  }
  //Add custom offset representation required analyses
  static void getAnalysisUsage(AnalysisUsage &AU)
  {
  
  }
  //Add custom offset representation init function
  static void initRepresentations()
  {
  
  }
  
  Offset operator+(const Offset& Other);
  Offset operator!=(const Offset& Other);
  void narrow(const NarrowingOp& narrowing_op);
  void widen(const WideningOp& widening_op);
  
  private:
  
  std::set<OffsetRepresentation*> reps;
};

}

#endif
