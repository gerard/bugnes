SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
SFOT_LIB=$(BUGNES_ROOT)/cpu/6502/libsfot.a
TARGET=a.out

CFLAGS+=`sdl-config --cflags`
LDFLAGS+=-lpthread `sdl-config --libs`

$(TARGET): $(OBJECTS) $(SFOT_LIB)
	$(CC) $(OBJECTS) $(SFOT_LIB) $(LDFLAGS) -o $@

clean:
	$(RM) $(OBJECTS) $(TARGET)
