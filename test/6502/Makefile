SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)

# Source asm files, compiled bins, srecord files and expected trace output
# to be used by the testing tool
ASMS=$(wildcard t/*.asm)
SRECS=$(ASMS:.asm=.srec)
BINS=$(ASMS:.asm=.bin)
TRACES=$(ASMS:.asm=.trace)

LDFLAGS+=-lpthread

SFOT_LIB=$(BUGNES_ROOT)/cpu/6502/libsfot.a
TARGET=a.out

all: $(TARGET)

$(TARGET): $(OBJECTS) $(SFOT_LIB)
	$(CC) $(OBJECTS) $(SFOT_LIB) $(LDFLAGS) -o $@

test: $(TARGET) $(BINS) $(TRACES)
	@./runtests.sh $(TARGET)

# crasm has a lot of crappy output
$(SRECS): %.srec: %.asm
	@crasm -s -l -x -o $@ $< > /dev/null

$(BINS): %.bin: %.srec
	@srec_cat $< -output $@ -binary

clean:
	$(RM) $(OBJECTS) $(TARGET) $(BINS) $(SRECS)

.PHONY: test
