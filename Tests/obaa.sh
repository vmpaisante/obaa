#!/bin/bash
opt -load OffsetBasedAliasAnalysis.so -obaa -aa-eval -stats -debug  $1.ssify.bc
