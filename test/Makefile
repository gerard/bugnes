SUBDIRS = smoke cpu/6502 bin/simplenes
SUBDIRS_POST = cov

LDFLAGS+=-lpthread $(CHECK_LIBS)

CHECK_DIRS= $(SUBDIRS)
CHECK_OBJS= $(patsubst %, %/check.o, $(CHECK_DIRS))
export GCOV_DIRS = $(shell pwd)/cpu/6502/obj $(shell pwd)/bin/simplenes/obj
export GCOV_OBJS = $(patsubst %, %/*.o, $(GCOV_DIRS))


TARGET=check_runner

all: subdirs test subdirs-post

$(TARGET): $(TARGET).o
	@$(CC) $(GCOV_CFLAGS) $(CFLAGS) $^ $(CHECK_OBJS) $(GCOV_OBJS) $(LDFLAGS) -o $@

test: $(TARGET)
	@./$(TARGET) | tail -n1

clean: subdirs-clean
	@rm -f $(TARGET) $(TARGET).o
	@echo CLEAN

include $(MAKE_INCDIR)/tail.mk
.PHONY: test
