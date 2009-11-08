SOURCES=main.c ppu.c
OBJECTS=$(SOURCES:.c=.o)
SFOT_LIB=$(BUGNES_ROOT)/cpu/6502/libsfot.a
TARGET=a.out

LDFLAGS+=-lpthread

$(TARGET): $(OBJECTS) $(SFOT_LIB)
	$(CC) $(OBJECTS) $(SFOT_LIB) $(LDFLAGS) -o $@

clean:
	$(RM) $(OBJECTS) $(TARGET)
