include build/head.mk
include build/cpu/6502.mk


# These part builds the coverage thingie
GCOV_DIRS = cpu/6502 bin/simplenes
GCOV_OBJS = $(wildcard $(patsubst %, %/obj/*.o, $(GCOV_DIRS)))
GCOV_REPORTS = $(patsubst %.o, %.c.gcov, $(GCOV_OBJS))

$(GCOV_REPORTS): FANCY_OBJ=$(subst obj/,,$(subst test/,,$(subst $(BUGNES_ROOT)/,,$<)))
$(GCOV_REPORTS): %.c.gcov: %.o
	@printf "GCOV\t$(FANCY_OBJ)\t"
	@if [ `echo $(FANCY_OBJ) | wc -c` -le 24 ]; then printf "\t"; fi
	@if [ `echo $(FANCY_OBJ) | wc -c` -le 16 ]; then printf "\t"; fi
	(cd `dirname $<` ; gcov $< | grep ^Lines; cd ..)

check.lcov: $(GCOV_REPORTS)
	@printf "LCOV\n"
	lcov $(patsubst %, --directory %, $(GCOV_DIRS)) --capture --output-file $@ > /dev/null

index.html: check.lcov
	@printf "HTML\t$@\n"
	mkdir -p html
	(cd html; genhtml ../$< > /dev/null)

# Standard build system definitions
ifndef V
FILTER_EXE_CHECK = tail -n1 | xargs -0 printf "RUN_TST\t%s"
endif

SUBDIRS  = smoke $(GCOV_DIRS)
OBJECTS  = $(GCOV_OBJS)
OBJECTS += $(wildcard $(patsubst %, %/check*.o, $(SUBDIRS)))
SOURCES_CHECK = check_runner.c

TARGET_CHECK = check_runner

EXTRA = run_test index.html
EXTRA_CLEAN = _html *.log *.lcov

include build/tail.mk
.PHONY: run_test
