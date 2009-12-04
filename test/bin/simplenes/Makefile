include $(BUGNES_ROOT)/build/head.mk

SOURCES_PATH_GCOV = $(BUGNES_ROOT)/$(subst $(BUGNES_ROOT)/test,,$(shell pwd))
COMPILE_PATH_GCOV = obj

CFLAGS += -I$(SOURCES_PATH_GCOV)

SOURCES_GCOV = hooks.c ppu.c ppu_registers.c
OBJECTS_GCOV = $(patsubst $(SOURCES), obj/%.o, $(TEST_SRC))

SOURCES_CHECK = check.c

include $(BUGNES_ROOT)/build/tail.mk
