# This could go in the parent makefile, but the problem is that the GCOV_OBJS
# expansion doesn't work there (the files don't exist => no expansion)

GCOV_OBJS:=	  $(wildcard $(GCOV_OBJS))
GCOV_REPORTS= $(patsubst %.o, %.c.gcov, $(GCOV_OBJS))


all: index.html

# grep the gcov output so it doesn't mess up the build log.  $FANCY_OBJ is the
# path from $BUGNES_ROOT to the real object file (not the instrumented, but the
# production one.
# The if line is just there to prettify the build output.
$(GCOV_REPORTS): FANCY_OBJ=$(subst obj/,,$(subst test/,,$(subst $(BUGNES_ROOT)/,,$<)))
$(GCOV_REPORTS): %.c.gcov: %.o
	@printf "GCOV\t$(FANCY_OBJ)\t"
	@if [ `echo $(FANCY_OBJ) | wc -c` -le 24 ]; then printf "\t"; fi
	@if [ `echo $(FANCY_OBJ) | wc -c` -le 16 ]; then printf "\t"; fi
	@(cd `dirname $<` ; gcov $< | grep ^Lines; cd ..)

check.lcov: $(GCOV_REPORTS)
	@printf "LCOV\n"
	@lcov $(patsubst %, --directory %, $(GCOV_DIRS)) --capture --output-file $@ > /dev/null

index.html: check.lcov
	@printf "HTML\t$@\n"
	@genhtml $< > /dev/null

# FIXME: The generated dir should be cpu/6502 instead of 6502
clean:
	@rm -rf cpu bin *.lcov *.png *.html *.css
	@echo CLEAN

include $(MAKE_INCDIR)/tail.mk
