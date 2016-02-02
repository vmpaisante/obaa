#!/bin/bash
opt -load RangeAnalysis.so -load OffsetBasedAliasAnalysis.so -basicaa -aa-eval -print-all-alias-modref-info -stats -debug  $1.essa.bc
