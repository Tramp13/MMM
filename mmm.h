#ifndef MMM_H
#define MMM_H

#include "raylib.h"

#define MAX_BEINGS 50

enum Tile {
    WATER,
    SAND,
    GRASS,
    TREE,
    NW_SAND_SE_WATER,
    NE_SAND_SW_WATER,
    SW_SAND_NE_WATER,
    SE_SAND_NW_WATER,
    NW_GRASS_SE_SAND,
    NE_GRASS_SW_SAND,
    SW_GRASS_NE_SAND,
    SE_GRASS_NW_SAND
};

typedef struct GameStruct Game;
struct GameStruct {
    int screen_w;
    int screen_h;
    int scale;
    int tile_size;
};

typedef struct EntityStruct Entity;
struct EntityStruct {
    int x;
    int y;
    int radius;
    int speed;
    Color color;
    int type;
    void (*update)();
};
Entity Entity_create(Game *game);
Entity Player_create(Game *game);

typedef struct MapStruct Map;
struct MapStruct {
    int w;
    int h;
    Entity *tiles;
    int being_count;
    Entity beings[MAX_BEINGS];
};

Map Map_create(int w, int h);
void Map_setTile(Map *map, int x, int y, int type, Game *Game);
Entity *Map_addBeing(Map *map);

#endif
