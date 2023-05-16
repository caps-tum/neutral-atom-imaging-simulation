CFLAGS=-lfftw3 -lm -fPIC -O3 -Iinclude
DLLFLAGS=-shared

SRC_DIR	:= src
OBJ_DIR	:= obj
BIN_DIR	:= bin

SOURCES := $(filter-out src/main.c,$(wildcard $(SRC_DIR)/*.c))
OBJECTS	:= $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: main so
fresh:
	-rm $(OBJECTS)
	make all
main: $(BIN_DIR)/main.o
so: $(BIN_DIR)/libcreateSampleImage.so
$(BIN_DIR)/libcreateSampleImage.so: $(OBJECTS) | $(BIN_DIR)
	$(CC) $(DLLFLAGS) -o $@ $^ $(CFLAGS)
	mkdir -p pip_project/neutral_atom_imaging_simulation/lib/
	cp -f $@ pip_project/neutral_atom_imaging_simulation/lib/
$(BIN_DIR)/main.o: $(SRC_DIR)/main.c $(OBJECTS) | $(BIN_DIR)
	$(CC) -o $@ $^ $(CFLAGS)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@
