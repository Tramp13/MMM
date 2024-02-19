#include <stdlib.h>
#include "mmm.h"
#include "raylib.h"

Map Map_create(int w, int h) {
    Map map;
    map.w = w;
    map.h = h;
    
    map.tiles = malloc(sizeof(int) * (w * h));
    for (int i = 0; i < w * h; i++) {
        map.tiles[i] = NO_TILE;
    }
    
    return map;
}

void Map_free(Map *map) {
    free(map->tiles);
}

int getTile(Map *map, int x, int y) {
    if (x >= 0 && y >= 0 && x < map->w && y < map->h) {
        return map->tiles[(y * map->w) + x];
    }
    return NO_TILE;
}

int getTileIndex(Map *map, int x, int y) {
    if (x >= 0 && y >= 0 && x < map->w && y < map->h) {
        return (y * map->w) + x;
    }
    return -1;
}

int getNorthTile(Map *map, int x, int y) {
    if (y > 0) {
	return map->tiles[((y - 1) * map->w) + x];
    }
    return NO_TILE;
}

int getEastTile(Map *map, int x, int y) {
    if (x < map->w - 1) {
	return map->tiles[(y * map->w) + (x + 1)];
    }
    return NO_TILE;
}

int getSouthTile(Map *map, int x, int y) {
    if (y < map->h - 1) {
	return map->tiles[((y + 1) * map->w) + x];
    }
    return NO_TILE;
}

int getWestTile(Map *map, int x, int y) {
    if (x > 0) {
	return map->tiles[(y * map->w) + (x - 1)];
    }
    return NO_TILE;
}

int getNorthEastTile(Map *map, int x, int y) {
    if (y > 0) {
	return map->tiles[((y - 1) * map->w) + (x + 1)];
    }
    return NO_TILE;
}

int getSouthEastTile(Map *map, int x, int y) {
    if (x < map->w - 1) {
	return map->tiles[((y + 1) * map->w) + (x + 1)];
    }
    return NO_TILE;
}

int getSouthWestTile(Map *map, int x, int y) {
    if (y < map->h - 1) {
	return map->tiles[((y + 1) * map->w) + (x - 1)];
    }
    return NO_TILE;
}

int getNorthWestTile(Map *map, int x, int y) {
    if (x > 0) {
	return map->tiles[((y - 1) * map->w) + (x - 1)];
    }
    return NO_TILE;
}

bool isSolid(Map *map, int x, int y) {
    int tile = getTile(map, x, y);
    if (tile == TREE || tile == FOREST_TREE || tile == WATER ||
        tile == DEEP_TREE) {
        return true;
    } else {
        return false;
    }
}
