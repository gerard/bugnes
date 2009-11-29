CFLAGS+=$(CHECK_CFLAGS) -I$(NES_SRC_DIR)
NES_SRC_DIR=$(BUGNES_ROOT)/bin/simplenes
NES_SRC= $(subst main.c, , $(wildcard $(NES_SRC_DIR)/*.c))
NES_OBJ= $(patsubst $(NES_SRC_DIR)/%.c, obj/%.o, $(NES_SRC))


all: check.o $(NES_OBJ)
	echo $(NES_OBJ)

$(NES_OBJ): obj/%.o: $(NES_SRC_DIR)/%.c
	@mkdir -p obj
	@printf "CC(COV)\t$@\n"
	@$(CC) $(CFLAGS) $(GCOV_CFLAGS) -c $< -o $@

clean:
	@rm -rf check.o obj
	@echo CLEAN


include $(MAKE_INCDIR)/tail.mk
