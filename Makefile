EXEC = maxforth
BIN_DIR = $(PWD)/bin
BUILD_DIR = $(PWD)/build
SRC_DIRS = src

SRCS = $(shell find $(SRC_DIRS) -name *.c)
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

INC_DIRS = include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS = $(INC_FLAGS) -Wall -MMD -MP
LDFLAGS =

CC = gcc

all: $(BIN_DIR)/$(EXEC)

$(BIN_DIR)/$(EXEC): $(OBJS)
	mkdir -p bin
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run format clean

run: $(BIN_DIR)/$(EXEC)
	$(BIN_DIR)/$(EXEC)

format:
	find src/* -exec clang-format -i {} \;

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)/$(EXEC)

-include $(DEPS)
