##===--------------------- Makefile ------------------------*- Makefile -*-===##
#
#         Offset Based Alias Analysis for The LLVM Compiler
#
##===----------------------------------------------------------------------===##

# Path to top level of LLVM hierarchy
 LEVEL = ../../..

# Name of the library to build
LIBRARYNAME = OffsetBasedAliasAnalysis

# Make the shared library become a loadable module so the tools can 
# dlopen/dlsym on the resulting library.
LOADABLE_MODULE = 1

CXXFLAGS += -std=c++0x 

# Include the makefile implementation stuff
include $(LEVEL)/Makefile.common
