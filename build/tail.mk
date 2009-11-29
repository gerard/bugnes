.PHONY: subdirs subdirs-clean clean

CHECK_CFLAGS=$(shell pkg-config --cflags check)
CHECK_LIBS=$(shell pkg-config --libs check)
GCOV_CFLAGS=-fprofile-arcs -ftest-coverage

subdirs: $(SUBDIRS)
	@for d in $(SUBDIRS); do (cd $$d; $(MAKE)); done

subdirs-post:
	@for d in $(SUBDIRS_POST); do (cd $$d; $(MAKE)); done

subdirs-clean: $(SUBDIRS)
	@for d in $(SUBDIRS); do (cd $$d; $(MAKE) clean); done
	@for d in $(SUBDIRS_POST); do (cd $$d; $(MAKE) clean); done
