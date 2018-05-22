PROJECT := bin/key_finder
SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=build/%.o)
LD := gcc
CFLAGS := -c
CC := gcc

all: $(PROJECT)

$(PROJECT): $(OBJ)
	mkdir -p bin
	$(CC) -o $(PROJECT) $(OBJ)

./build/main.o: ./src/main.c ./include/padre.h ./include/types.h
	mkdir -p build
	$(CC) $(CFLAGS) $< -o $@

./build/logger.o: ./src/logger.c ./include/logger.h ./include/types.h
	mkdir -p build
	$(CC) $(CFLAGS) $< -o $@

./build/padre.o: ./src/padre.c ./include/padre.h ./include/logger.h
	mkdir -p build
	$(CC) $(CFLAGS) $< -o $@

./build/figlio.o: ./src/figlio.c ./include/figlio.h ./include/types.h
	mkdir -p build
	$(CC) $(CFLAGS) $< -o $@

./build/nipote.o: ./src/nipote.c ./include/nipote.h ./include/types.h
	mkdir -p build
	$(CC) $(CFLAGS) $< -o $@

install:
	mkdir -p bin
	cp $(PROJECT) bin/

clean:
	rm $(OBJ) $(PROJECT)

help:
	@echo all: compiles all files
	@echo install: installs application at right place
	@echo clean: delets everything except source file
	@echo doc: generate documentation

doc:
	doxygen -g
	

	.PHONY clean all install