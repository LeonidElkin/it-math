include config.cfg

BLOCKY = $(BLD_DIR)$(NAME_1)
SEQUANTIAL = $(BLD_DIR)$(NAME_2)
PARALLEL = $(BLD_DIR)$(NAME_3)

BLOCKY_OBJ = $(BLD_DIR)$(NAME_1).o
SEQUANTIAL_OBJ = $(BLD_DIR)$(NAME_2).o
PARALLEL_OBJ = $(BLD_DIR)$(NAME_3).o

BLOCKY_SRC = $(SRC_DIR)/$(NAME_1).c
SEQUANTIAL_SRC = $(SRC_DIR)/$(NAME_2).c
PARALLEL_SRC = $(SRC_DIR)/$(NAME_3).c
 
LBR_SRC = $(wildcard $(LBR_DIR)*.c)
LBR_OBJ = $(patsubst $(LBR_DIR)%.c, $(BLD_DIR)%.o, $(LBR_SRC))

.PHONY: all clean sequantial parallel build help

## make: Build a parallel block algorithm
all: build $(BLOCKY)
	@echo "A parallel block algorithm is built"

## make sequantial: Build a sequantial algorithm
sequantial: build $(SEQUANTIAL)
	@echo "A sequantial algorithm is built"

## make parallel: Build a parallel algorithm without the block method
parallel: build $(PARALLEL)
	@echo "A parallel algorithm is built"

$(PARALLEL): $(PARALLEL_OBJ) $(LBR_OBJ)
	@$(CC) $(FLAGS) $^ -o $@

$(PARALLEL_OBJ): $(PARALLEL_SRC)
	@$(CC) $(FLAGS) -c $^ -o $@

$(SEQUANTIAL): $(SEQUANTIAL_OBJ) $(LBR_OBJ)
	@$(CC) $(FLAGS) $^ -o $@

$(SEQUANTIAL_OBJ): $(SEQUANTIAL_SRC)
	@$(CC) $(FLAGS) -c $^ -o $@

$(BLOCKY): $(BLOCKY_OBJ) $(LBR_OBJ)
	@$(CC) $(FLAGS) $^ -o $@

$(BLOCKY_OBJ): $(BLOCKY_SRC)
	@$(CC) $(FLAGS) -c $^ -o $@

$(BLD_DIR)%.o: $(LBR_DIR)/%.c
	@$(CC) $(FLAGS) -c $< -o $@

build:
	@mkdir -p $(BLD_DIR)

## make clean: Delete build artifacts
clean:
	@rm -rf $(BLD_DIR)

help: Makefile
	@echo " Choose a command run:"
	@sed -n 's/^##//p' $< | column -t -s ':' |  sed -e 's/^/ /'