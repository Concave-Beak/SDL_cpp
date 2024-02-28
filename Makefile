CC = g++
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lSDL2 -lSDL2_ttf
SRC_DIR = Source
BUILD_DIR = Build
BIN_DIR = Bin

SRCS = $(wildcard $(SRC_DIR)/*/*.cc)
OBJS = $(patsubst $(SRC_DIR)/*/%.cc, $(BUILD_DIR)/%.o, $(SRCS))
TARGET = $(BIN_DIR)/project

$(TARGET): $(OBJS) 
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean

all: $(TARGET)

force-rebuild:
	@rm -f $(OBJS)

rebuild: clean all

touch:
	@touch $(SRCS)
