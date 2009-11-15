SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst src/%, obj/%, $(SOURCES:.c=.o))
OUT=libsfot.a

CFLAGS+=-I./include -g

all: $(OUT)

obj/%.o : src/%.c
	@mkdir -p obj
	@printf "CC\t$@\n"
	@$(CC) -c $(CFLAGS) $< -o $@

$(OUT): $(OBJECTS)
	@printf "AR\t$@\n"
	@ar cq $(OUT) $(OBJECTS)

clean:
	@echo CLEAN
	@$(RM) -r obj $(OUT)
