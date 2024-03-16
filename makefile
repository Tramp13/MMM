OBJS = main.c entity.c player.c perlin.c map.c overworld.c lab.c puzzlebox.c puzzledungeon.c

CC = gcc

#COMPILER_FLAGS = -w

LINKER_FLAGS = -lraylib -lm -lncurses

OBJ_NAME = main

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -DPLATFORM_DESKTOP -o $(OBJ_NAME)
