SOURCES=main.c ppu.c ppu_registers.c
OBJECTS=$(SOURCES:.c=.o)
SFOT_LIB=$(BUGNES_ROOT)/cpu/6502/libsfot.a
TARGET=a.out

LDFLAGS+=-lpthread

$(TARGET): $(OBJECTS) $(SFOT_LIB)
	@printf "LD\t$@\n"
	@$(CC) $(OBJECTS) $(SFOT_LIB) $(LDFLAGS) -o $@

clean:
	@echo CLEAN
	@$(RM) $(OBJECTS) $(TARGET)

include $(BUGNES_ROOT)/build/tail.mk
