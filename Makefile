PROJECT := build/key_finder
SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=build/%.o)
LD := gcc
CFLAGS := -I include/ -c
CC := gcc
ex: EX=thread

all: $(PROJECT)

$(PROJECT): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(PROJECT) -lpthread

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) $(EX) $< -o $@

install:
	mkdir -p bin
	cp $(PROJECT) bin/

help:
	@echo all: compiles all files
	@echo thread: compile thread version
	@echo install: installs application at right place
	@echo clean: delets everything except source file
	@echo doc: generate documentation

thread: EX := -D CONDITION=1
thread: all
	
doc:
	doxygen doxygen.cfg

clean:
	rm $(OBJ) $(PROJECT)
	
.PHONY: all clean install