PROJECT := build/key_finder
SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=build/%.o)
LD := gcc
CFLAGS := -c
CC := gcc
ex: EX=thread

all: $(PROJECT)

ifeq ($(EX),thread)
$(PROJECT): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(PROJECT) -lpthread

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -D CONDITION=1 $< -o $@
else
$(PROJECT): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(PROJECT)

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) $< -o $@
endif

install:
	mkdir -p bin
	cp $(PROJECT) bin/

help:
	@echo all: compiles all files
	@echo install: installs application at right place
	@echo clean: delets everything except source file
	@echo doc: generate documentation

thread:
	make EX=thread

doc:
	doxygen doxygen.cfg

clean:
	rm $(OBJ) $(PROJECT)
	

.PHONY: all clean install