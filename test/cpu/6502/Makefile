include build/head.mk
include build/cpu/6502.mk


COMPILE_PATH_GCOV = obj
SOURCES_PATH_GCOV = $(TESTDIR_TO_IMP)/src
SOURCES_GCOV = addr.c core.c disas.c excepthooks.c memhooks.c opcodes.c sfot.c

SOURCES_CHECK = check.c

# This uses internals, so cpu/6502.mk is not enough
CFLAGS += -I$(TESTDIR_TO_IMP)/include

include build/tail.mk
