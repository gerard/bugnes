# This is to be included for every module using this library

CFLAGS += -I$(BUGNES_ROOT)/include
LDFLAGS += -lpthread
TARGET_LIBS += $(BUGNES_ROOT)/cpu/6502/libsfot.a
