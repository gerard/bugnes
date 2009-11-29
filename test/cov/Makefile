# This could go in the parent makefile, but the problem is that the GCOV_OBJS
# expansion doesn't work there (the files don't exist => no expansion)

GCOV_DIRS=	  ../cpu/6502/obj
GCOV_OBJS=	  $(shell echo $(patsubst %, %/*.o, $(GCOV_DIRS)))
GCOV_REPORTS= $(patsubst %.o, %.c.gcov, $(GCOV_OBJS))


all: index.html

# grep the gcov output so it doesn't mess up the build log.  The if line helps
# pretifying the output also.
$(GCOV_REPORTS): %.c.gcov: %.o
	@printf "GCOV\t$<\t"
	@if [ `echo $< | cut -d\/ -f2- | wc -c` -lt 23 ]; then printf "\t"; fi
	@(cd `dirname $<` ; gcov $< | grep ^Lines; cd ..)

check.lcov: $(GCOV_REPORTS)
	@printf "LCOV\n"
	@lcov --directory $(GCOV_DIRS) --capture --output-file $@ > /dev/null

index.html: check.lcov
	@printf "HTML\t$@\n"
	@genhtml $< > /dev/null

# FIXME: The generated dir should be cpu/6502 instead of 6502
clean:
	@rm -rf 6502 *.lcov *.png *.html *.css
	@echo CLEAN

include $(MAKE_INCDIR)/tail.mk
