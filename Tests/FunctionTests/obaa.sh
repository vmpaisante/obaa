#!/bin/bash
opt -load OffsetBasedAliasAnalysis.so -obaa -aa-eval -print-all-alias-modref-info -stats -debug  $1.essa.bc
