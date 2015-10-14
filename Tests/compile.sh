#!/bin/bash
clang -c -emit-llvm $1.c -o $1.bc
llvm-dis $1.bc -o $1.clang.ll
sage-opt -load SSIfy.so -mem2reg -instnamer -break-crit-edges -ssify -set 1000  $1.bc -o $1.ssify.bc
llvm-dis $1.ssify.bc -o $1.ssify.ll
