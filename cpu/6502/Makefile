SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst src/%, obj/%, $(SOURCES:.c=.o))
OUT=libsfot.a

CFLAGS+=-I./include -g

all: $(OUT)

obj/%.o : src/%.c
	@mkdir -p obj
	$(CC) -c $(CFLAGS) $< -o $@

$(OUT): $(OBJECTS)
	ar cq $(OUT) $(OBJECTS)

clean:
	$(RM) -r obj $(OUT)
