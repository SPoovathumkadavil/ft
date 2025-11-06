
APP_NAME = ft

GLOBAL_LIB_DIR = $(HOME)/.local/lib
GLOBAL_LIBRARY_DIR = $(HOME)/.local/library
GLOBAL_INC_DIR = $(HOME)/.local/include
INSTALL_DIR = $(HOME)/.local/bin
LIB_DIR = lib
LIBRARY_DIR = library
SRC_DIR = src
OBJ_DIR = build
TARGET_DIR = $(OBJ_DIR)
INC_DIR = include
SRC_FILES = $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

LIBS := $(notdir $(wildcard $(LIB_DIR)/*))
LIBNAMES := $(patsubst lib%.so, %, $(LIBS))

CC = g++
CFLAGS = -std=c++20 -Wall -fPIC -I$(GLOBAL_INC_DIR) -I$(INC_DIR)
LDFLAGS = # -L$(GLOBAL_LIB_DIR) -L$(LIB_DIR) $(addprefix -l,$(LIBNAMES))
TARGET = $(TARGET_DIR)/$(APP_NAME)

.PHONY: compile_db
compile_db:
	$(MAKE) clean
	bear -- $(MAKE) build

build: $(TARGET) build-tr

build-tr:
	cd tr && $(MAKE) build

run:
	$(TARGET)

install: uninstall install-tr
	mkdir -p $(GLOBAL_LIB_DIR)
	mkdir -p $(GLOBAL_INC_DIR)
	mkdir -p $(INSTALL_DIR)
	cp $(TARGET) $(INSTALL_DIR)

install-tr:
	cd tr && $(MAKE) install

uninstall: uninstall-tr
	rm -f $(INSTALL_DIR)/$(notdir $(TARGET))
	rm -rf $(GLOBAL_LIBRARY_DIR)/$(APP_NAME)

uninstall-tr:
	cd tr && $(MAKE) uninstall

# Rule to build the shared library
$(TARGET): $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $(OBJ_FILES)

# Rule to compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean: clean-tr
	rm -f $(OBJ_FILES) $(TARGET)

clean-tr:
	cd tr && $(MAKE) clean

