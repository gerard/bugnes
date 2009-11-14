.PHONY: subdirs subdirs-clean subdirs-test

CHECK_CFLAGS=$(shell pkg-config --cflags check)
CHECK_LIBS=$(shell pkg-config --libs check)


subdirs: $(SUBDIRS)
	@for d in $(SUBDIRS); do (cd $$d; $(MAKE)); done

subdirs-clean: $(SUBDIRS)
	@for d in $(SUBDIRS); do (cd $$d; $(MAKE) clean); done
	@for d in $(SUBDIRS_TEST); do (cd $$d; $(MAKE) clean); done

subdirs-test:
	@for d in $(SUBDIRS_TEST); do (cd $$d; $(MAKE) test); done
