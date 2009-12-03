# The meat of this is the default rules for different types of C files we are
# facing (normal, coverage, unit tests).  There are also rules for recursing
# into directories.


# If SOURCES is defined, using SOURCES_PATH is just going to screw up patsubst
# so we do this instead
ifdef SOURCES
SOURCES_PATH ?= .
COMPILE_PATH ?= .
COMPILE_OBJS = $(patsubst %.c, $(COMPILE_PATH)/%.o, $(SOURCES))
endif

# Compilation with coverage
ifdef SOURCES_GCOV
SOURCES_PATH_GCOV ?= .
COMPILE_PATH_GCOV ?= .
COMPILE_OBJS_GCOV = $(patsubst %.c, $(COMPILE_PATH_GCOV)/%.o, $(SOURCES_GCOV))
CFLAGS_GCOV = -fprofile-arcs -ftest-coverage

# These files are generated at compilation time
GCOV_GCNO = $(patsubst %.c, $(COMPILE_PATH_GCOV)/%.gcno, $(SOURCES_GCOV))
endif

# Compilation of unit test material
ifdef SOURCES_CHECK
SOURCES_PATH_CHECK ?= .
COMPILE_PATH_CHECK ?= .
COMPILE_OBJS_CHECK = $(patsubst %.c, $(COMPILE_PATH_CHECK)/%.o, $(SOURCES_CHECK))
CFLAGS_CHECK = $(shell pkg-config --cflags check)

# Not technically a check LDFLAG, but at least is always necessary in this
# project (we need coverage in the tests).
LDFLAGS_CHECK = -fprofile-arcs -ftest-coverage
LIBS_CHECK = $(shell pkg-config --libs check)
endif

# Convenience definition
COMPILE_ALL = $(COMPILE_OBJS) $(COMPILE_OBJS_GCOV) $(COMPILE_OBJS_CHECK)
OBJECTS_ALL = $(OBJECTS) $(COMPILE_OBJS) $(COMPILE_OBJS_GCOV) $(COMPILE_OBJS_CHECK)

# Default compile rule
$(COMPILE_OBJS): $(COMPILE_PATH)/%.o : $(SOURCES_PATH)/%.c
	@printf "CC\t$@\n"
	$(MKDIR) $(COMPILE_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

# Default compile rule with coverage
$(COMPILE_OBJS_GCOV): $(COMPILE_PATH_GCOV)/%.o : $(SOURCES_PATH_GCOV)/%.c
	@printf "CC(COV)\t$@\n"
	$(MKDIR) $(COMPILE_PATH_GCOV)
	$(CC) $(CFLAGS) $(CFLAGS_GCOV) -c $< -o $@

# Default compile rule for check objects
$(COMPILE_OBJS_CHECK): $(COMPILE_PATH_CHECK)/%.o : $(SOURCES_PATH_CHECK)/%.c
	@printf "CC(CHK)\t$@\n"
	$(MKDIR) $(COMPILE_PATH_CHECK)
	$(CC) $(CFLAGS) $(CFLAGS_CHECK) -c $< -o $@


# Only one of these three might be in one Makefile
# Default linking rule
$(TARGET): $(OBJECTS_ALL) $(TARGET_LIBS)
	@printf "LD\t$@\n"
	$(CC) $^ $(LDFLAGS) -o $@

# Default linking rule: unit test (also exec)
FILTER_EXE_CHECK ?= cat
$(TARGET_CHECK): $(OBJECTS_ALL) $(TARGET_LIBS)
	@printf "LD\t$@\n"
	$(CC) $^ $(LIBS_CHECK) $(LDFLAGS_CHECK) $(LDFLAGS) -o $@
	./$(TARGET_CHECK) | $(FILTER_EXE_CHECK)


# Default static library rule
$(STATIC_LIBRARY): $(OBJECTS_ALL)
	@printf "AR\t$@\n"
	$(AR) $(STATIC_LIBRARY) $(OBJECTS_ALL)


# Cleaning rule: it prints PROJECT_PATH itself since having a DIR and a CLEAN
# line would be too verbose
clean: subdirs-clean
	@printf "CLEAN\t$(PROJECT_PATH)\n"
	$(RM) $(COMPILE_OBJS) $(COMPILE_OBJS_GCOV) $(COMPILE_OBJS_CHECK)
	$(RM) $(GCOV_GCNO)
	$(RM) $(TARGET) $(TARGET_CHECK) $(STATIC_LIBRARY)
	$(RM) $(EXTRA_CLEAN)
	$(RMDIR) $(COMPILE_PATH) || true
	$(RMDIR) $(COMPILE_PATH_GCOV) || true
	$(RMDIR) $(COMPILE_PATH_CHECK) || true


# Recursing into directories
subdirs: $(SUBDIRS)
	for d in $(SUBDIRS); do (cd $$d; $(MAKE)); done

subdirs-post:
	for d in $(SUBDIRS_POST); do (cd $$d; $(MAKE)); done

subdirs-clean: $(SUBDIRS)
	for d in $(SUBDIRS); do (cd $$d; $(MAKE) clean); done
	for d in $(SUBDIRS_POST); do (cd $$d; $(MAKE) clean); done


self: $(OBJECTS_ALL) $(STATIC_LIBRARY) $(TARGET) $(TARGET_CHECK) $(EXTRA) subdirs-post
.PHONY: subdirs subdirs-clean subdirs-post clean
