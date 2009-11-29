CFLAGS+=-I$(BUGNES_ROOT)/include $(CHECK_CFLAGS)
LDFLAGS+=-lpthread
SFOT_LIB=$(BUGNES_ROOT)/cpu/6502/libsfot.a
SFOT_SRC_DIR=$(BUGNES_ROOT)/cpu/6502/src
SFOT_SRC= $(wildcard $(SFOT_SRC_DIR)/*.c)
SFOT_OBJ= $(patsubst $(SFOT_SRC_DIR)/%.c, obj/%.o, $(SFOT_SRC))


all: check.o $(SFOT_OBJ)

$(SFOT_OBJ): obj/%.o: $(SFOT_SRC_DIR)/%.c
	@mkdir -p obj
	@printf "CC(COV)\t$@\n"
	@$(CC) $(CFLAGS) $(GCOV_CFLAGS) -I$(BUGNES_ROOT)/cpu/6502/include -c $< -o $@

clean:
	@rm -rf *.o obj
	@echo CLEAN

include $(MAKE_INCDIR)/tail.mk
