CFLAGS=-g -I$(BUGNES_ROOT)/include $(CHECK_CFLAGS)
LDFLAGS=-lpthread
SFOT_LIB=$(BUGNES_ROOT)/cpu/6502/libsfot.a

TARGET=check_runner

check_runner: check.o $(SFOT_LIB)
	@printf "LD\t$@\n"
	@gcc $(LDFLAGS) $^ $(CHECK_LIBS) -o $@

all: test
test: $(TARGET)
	@./$(TARGET)

clean:
	@rm -rf $(TARGET) *.o
	@echo CLEAN

include $(MAKE_INCDIR)/tail.mk
