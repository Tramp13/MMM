#ifndef MMM_H
#define MMM_H

#include "raylib.h"

#define MAX_BEINGS 50

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
