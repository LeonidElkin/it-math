include config.cfg

CC = gcc

TARGET = $(BLD_DIR)$(NAME_1)

OBJ = $(BLD_DIR)$(NAME_1).o

SRC = $(SRC_DIR)/$(NAME_1).c

FLAGS = -lm -fopenmp -Wall -Wextra -Werror
LBR_SRC = $(wildcard $(LBR_DIR)*.c)
LBR_OBJ = $(patsubst $(LBR_DIR)%.c, $(BLD_DIR)%.o, $(LBR_SRC))

.PHONY: all clean sequantial parallel_non_block build

all: build $(TARGET)

$(TARGET): $(OBJ) $(LBR_OBJ)
	$(CC) $(FLAGS) $^ -o $@

$(OBJ): $(SRC)
	$(CC) $(FLAGS) -c $^ -o $@

$(BLD_DIR)%.o: $(LBR_DIR)/%.c
	$(CC) $(FLAGS) -c $< -o $@

build:
	mkdir -p $(BLD_DIR)

clean:
	rm -rf $(BLD_DIR)