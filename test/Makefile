CFLAGS+=-I$(BUGNES_ROOT)/include $(CHECK_CFLAGS)
LDFLAGS+=-lpthread
SFOT_LIB=$(BUGNES_ROOT)/cpu/6502/libsfot.a
SFOT_SRC_DIR=$(BUGNES_ROOT)/cpu/6502/src
SFOT_SRC=$(wildcard $(SFOT_SRC_DIR)/*.c)
SFOT_OBJ=$(patsubst $(SFOT_SRC_DIR)/%.c, obj/%.o, $(SFOT_SRC))
SFOT_GCOV=$(patsubst obj/%.o, obj/%.c.gcov, $(SFOT_OBJ))

TARGET=check_runner

all: test html_cov

# grep the gcov output so it doesn't mess up the build log.  The if line helps
# pretifying the output also.
$(SFOT_GCOV): %.c.gcov: %.o
	@printf "GCOV\t$<\t"
	@if [ `echo $< | wc -c` -lt 16 ]; then printf "\t"; fi
	@(cd obj; gcov $< | grep ^Lines; cd ..)

html_cov: check.lcov
	@printf "HTML\t$@\n"
	@genhtml -o $@ $< > /dev/null > /dev/null

check.lcov: $(SFOT_GCOV)
	@printf "LCOV\n"
	@lcov --directory obj --capture --output-file $@ > /dev/null

$(SFOT_OBJ): obj/%.o: $(SFOT_SRC_DIR)/%.c
	@mkdir -p obj
	@printf "CC(COV)\t$@\n"
	@$(CC) $(GCOV_CFLAGS) $(CFLAGS) -c $< -o $@ -I$(BUGNES_ROOT)/include \
			-I$(BUGNES_ROOT)/cpu/6502/include

check_runner: $(SFOT_OBJ) check.o
	@printf "LD\t$@\n"
	@gcc $(GCOV_CFLAGS) $(LDFLAGS) $^ $(CHECK_LIBS) -o $@

test: $(TARGET)
	@./$(TARGET)

clean:
	@rm -rf $(TARGET) *.o check.lcov html_cov obj
	@echo CLEAN

include $(MAKE_INCDIR)/tail.mk
