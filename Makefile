# Settings for folders
OBJECT_DIR  = obj
LIBRARY_DIR = lib
SOURCE_DIR  = src

# Compilers options
CC          = g++
CFLAGS      = -I$(LIBRARY_DIR) -O3 -ggdb
LIBS        = -lgmp -lgmpxx -lssl -lcrypto

# Requirements and stuffs
FULLDEPS := $(shell find $(LIBRARY_DIR) -name '*.h')
FULLOBJS := $(shell find $(SOURCE_DIR) -name '*.cpp' | sed -e "s/^$(SOURCE_DIR)/$(OBJECT_DIR)/" | sed -e "s/\\.cpp$$/.o/")

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(FULLDEPS) | $(OBJECT_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

proto-proof: $(OBJECT_DIR)/proto-proof.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# For commands
.PHONY: clean run dbg gdb
clean:
	rm -f $(SOURCE_DIR)/*.o $(OBJECT_DIR)/*.o ./main ./proto-proof

test: proto-proof
	./proto-proof

dbg-test: proto-proof
	gdb-pwndbg ./proto-proof

gdb-test: proto-proof
	gdb ./proto-proof
