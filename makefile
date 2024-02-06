OBJS = main.c entity.c player.c perlin.c map.c

CC = gcc

#COMPILER_FLAGS = -w

LINKER_FLAGS = -lraylib -lm

OBJ_NAME = main

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
