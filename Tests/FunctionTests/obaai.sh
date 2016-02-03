#!/bin/bash
opt -load OffsetBasedAliasAnalysis.so -obaa -aa-eval -inter -print-all-alias-modref-info -stats -debug  $1.essa.bc
