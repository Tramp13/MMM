#ifndef MMM_H
#define MMM_H

#include "raylib.h"

#define MAX_BEINGS 50

enum Tile {
    NO_TILE,
    WATER,
    SAND,
    GRASS,
    FOREST_FLOOR,
    TREE,
    FOREST_TREE,
    DEEP_TREE,
    STAIRS,
    STONE_WALL,
    STONE_FLOOR,
    DOOR
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
    int **tiles;
    int being_count;
    Entity beings[MAX_BEINGS];
};

Map Map_create(int w, int h);
void Map_free(Map *map);
void Map_copy(Map *map_a, Map *map_b);
int getTile(Map *map, int x, int y);
int getTileIndex(Map *map, int x, int y);
int getNorthTile(Map *map, int x, int y);
int getEastTile(Map *map, int x, int y);
int getSouthTile(Map *map, int x, int y);
int getWestTile(Map *map, int x, int y);
int getNorthEastTile(Map *map, int x, int y);
int getSouthEastTile(Map *map, int x, int y);
int getSouthWestTile(Map *map, int x, int y);
int getNorthWestTile(Map *map, int x, int y);
bool isSolid(Map *map, int x, int y);

Map Map_createLab();

void Map_perlinify(Map *map, int seed);
void Map_enhanceForests(Map *map);
#endif
