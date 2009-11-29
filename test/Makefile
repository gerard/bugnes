SUBDIRS = cpu/6502 smoke
SUBDIRS_POST = cov

LDFLAGS+=-lpthread $(CHECK_LIBS)

CHECK_DIRS= cpu/6502 smoke
CHECK_OBJS= $(patsubst %, %/check.o, $(CHECK_DIRS))
GCOV_DIRS = cpu/6502/obj
GCOV_OBJS = $(patsubst %, %/*.o, $(GCOV_DIRS))


TARGET=check_runner

all: subdirs test subdirs-post

$(TARGET): $(TARGET).o
	@$(CC) $(GCOV_CFLAGS) $(CFLAGS) $^ $(CHECK_OBJS) $(GCOV_OBJS) $(LDFLAGS) -o $@

test: $(TARGET)
	@./$(TARGET)

clean: subdirs-clean
	@rm -f $(TARGET) $(TARGET).o
	@echo CLEAN

include $(MAKE_INCDIR)/tail.mk
.PHONY: test
