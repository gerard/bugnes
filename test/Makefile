SUBDIRS_TEST=6502
CFLAGS=-I$(BUGNES_ROOT)/include $(CHECK_CFLAGS)
LDFLAGS=-lpthread
SFOT_LIB=$(BUGNES_ROOT)/cpu/6502/libsfot.a

check_runner: check.o $(SFOT_LIB)
	@printf "LD\t$@\n"
	@gcc $(LDFLAGS) $^ $(CHECK_LIBS) -o $@

all: subdirs
test: subdirs-test check_runner
	@./check_runner

clean: subdirs-clean

include $(MAKE_INCDIR)/tail.mk
