##===- TEST.ftobaa.Makefile -----------------------------*- Makefile -*-===##
#
# Usage: 
#     make TEST=ftobaa (detailed list with time passes, etc.)
#     make TEST=ftobaa report
#     make TEST=ftobaa report.html
#
##===----------------------------------------------------------------------===##

CURDIR  := $(shell cd .; pwd)
PROGDIR := $(PROJ_SRC_ROOT)
RELDIR  := $(subst $(PROGDIR),,$(CURDIR))

#LLVM_DIR = "/home/vitor/Ecosoc"
#LLVM_BUILD = "Release+Asserts"


$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.$(TEST).report.txt
	@cat $<

$(PROGRAMS_TO_TEST:%=Output/%.$(TEST).report.txt):  \
Output/%.$(TEST).report.txt: Output/%.linked.rbc $(LOPT) \
	$(PROJ_SRC_ROOT)/TEST.ftobaa.Makefile 
	$(VERB) $(RM) -f $@
	@echo "---------------------------------------------------------------" >> $@
	@echo ">>> ========= '$(RELDIR)/$*' Program" >> $@
	@echo "---------------------------------------------------------------" >> $@
	@opt -load vSSA.so -mem2reg -instnamer -break-crit-edges -vssa $< -o $<.essa.bc 2>>$@
	@opt -load OffsetBasedAliasAnalysis.so -obaa -load AliasSets.so -load DepGraph.so -load bSSA2.so -bssa2 -xmlfile /home/vitor/in.xml -stats -time-passes $<.essa.bc -o $<.essa.bc 2>>$@ 


