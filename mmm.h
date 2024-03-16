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

enum PuzzleType {
    NO_PUZZLE = 0,
    KEY = 1,
    FIGHT = 2
};

enum Cardinal {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
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
Vector2 Map_findRandomTile(Map *map, int tile_type);

Map Map_createLab();
Map Map_createPuzzleDungeon();

void Map_perlinify(Map *map, int seed);
void Map_enhanceForests(Map *map);

typedef struct PuzzleRoomStruct PuzzleRoom;
struct PuzzleRoomStruct {
    int entrance; // Represents room index in PuzzleBoxStruct of the room that led to this one
    int neighbors[4]; // Index should be NORTH, EAST, SOUTH, or WEST. Value represents room index in PuzzleBoxStruct
    int lock_type;
    int puzzle_type;
    int x, y;
};

typedef struct PuzzleBoxStruct PuzzleBox;
struct PuzzleBoxStruct {
    int room_count;
    int entrance[20];
    int lock_type[20];
    int puzzle_type[20];
    int x[20];
    int y[20];
};

PuzzleBox PuzzleBox_create();
#endif
