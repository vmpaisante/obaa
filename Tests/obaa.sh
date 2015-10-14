#!/bin/bash
opt -load OffsetBasedAliasAnalysis.so -obaa -aa-eval -print-all-alias-modref-info  $1.ssify.bc
