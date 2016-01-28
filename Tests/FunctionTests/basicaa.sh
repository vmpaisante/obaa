#!/bin/bash
opt -load OffsetBasedAliasAnalysis.so -basicaa -aa-eval -print-all-alias-modref-info -stats -debug  $1.ssify.bc
