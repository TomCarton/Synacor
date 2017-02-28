# Makefile

TARGET := synacor

OBJ_DIR := obj

C_FILES := $(wildcard sources/*.c)
OBJ_FILES := $(addprefix $(OBJ_DIR)/,$(notdir $(C_FILES:.c=.o)))

CC=clang
CC_FLAGS = -Wall -O3


all: $(OBJ_FILES)
	$(CC) $(CC_FLAGS) $^ -o $(TARGET)

$(OBJ_DIR)/%.o: sources/%.c
	@echo Compile "$@"...
	$(CC) $(CC_FLAGS) -c $^ -o $@


$(OBJ_FILES): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)


.PHONY: clean
clean:
	rm $(TARGET)
	rm -rf $(OBJ_DIR)


.PHONY: info
info:
	@echo sources: $(C_FILES)
	@echo objects: $(OBJ_FILES)
