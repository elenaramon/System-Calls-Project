PROJECT := soluzione
SRC := $(wildcard *.c)
OBJ := $(SRC:%.c=obj/%.o)
LD := gcc
CFLAGS := -c
CC := gcc

all: $(PROJECT)

$(PROJECT): $(OBJ)
	$(CC) -o $(PROJECT) $(OBJ)

./obj/main.o: main.c ./h/from_int_to_string.h ./h/string_length.h ./h/concat_string.h
	mkdir -p obj
	$(CC) $(CFLAGS) $< -o $@

./obj/concat_string.o: concat_string.c ./h/concat_string.h ./h/string_length.h
	mkdir -p obj
	$(CC) $(CFLAGS) $< -o $@

./obj/from_int_to_string.o: from_int_to_string.c ./h/from_int_to_string.h
	mkdir -p obj
	$(CC) $(CFLAGS) $< -o $@

./obj/string_length.o: string_length.c ./h/string_length.h
	mkdir -p obj
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJ)


help:
	@echo all: compiles all files
	@echo install: installs application at right place
	@echo clean: delets everything except source file
	@echo doc: generate documentation

doc:
	doxygen -g
	

	.PHONY clean all install