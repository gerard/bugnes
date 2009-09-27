.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)
	@for d in $(SUBDIRS); do (cd $$d; $(MAKE)); done

subdirs-clean: $(SUBDIRS)
	@for d in $(SUBDIRS); do (cd $$d; $(MAKE) clean); done
