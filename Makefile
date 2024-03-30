# Compiler and flags
CC := g++
CFLAGS := -Wall -Wextra -O2
LDFLAGS := -lSDL2 -lSDL2_ttf

# Directories
SRC_DIR := Source
BUILD_DIR := Build
BIN_DIR := Bin

# Find source files
SRCS := $(wildcard $(SRC_DIR)/**/*.cc ./Include/Utils/*)
OBJS := $(patsubst $(SRC_DIR)/%.cc, $(BUILD_DIR)/%.o, $(SRCS))

# Target
TARGET := $(BIN_DIR)/project

# Make all by default
all: $(TARGET)

# Rule to generate executable
$(TARGET): $(OBJS) 
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Phony targets
.PHONY: all clean rebuild

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Rebuild
rebuild: clean all
