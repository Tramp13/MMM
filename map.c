#include <stdlib.h>
#include "mmm.h"
#include "raylib.h"

Map Map_create(int w, int h) {
    Map map;
    map.w = w;
    map.h = h;
    
    map.tiles = (int**) malloc(h * sizeof(int*));

    for (int y = 0; y < h; y++) {
        map.tiles[y] = (int*) malloc(w * sizeof(int));
        for (int x = 0; x < w; x++) {
            map.tiles[y][x] = NO_TILE;
        }
    }
    
    return map;
}

void Map_free(Map *map) {
    for (int y = 0; y < map->h; y++) {
        free(map->tiles[y]);
    }
    free(map->tiles);
}

void Map_copy(Map *map_dst, Map *map_src) {
    for (int y = 0; y < map_src->h; y++) {
        for (int x = 0; x < map_src->w; x++) {
            map_dst->tiles[y][x] = map_src->tiles[y][x];
        }
    }
}
            

int getTile(Map *map, int x, int y) {
    if (x >= 0 && y >= 0 && x < map->w && y < map->h) {
        return map->tiles[y][x];
    }
    return NO_TILE;
}

int getNorthTile(Map *map, int x, int y) {
    if (y > 0) {
	return map->tiles[y - 1][x];
    }
    return NO_TILE;
}

int getEastTile(Map *map, int x, int y) {
    if (x < map->w - 1) {
	return map->tiles[y][x + 1];
    }
    return NO_TILE;
}

int getSouthTile(Map *map, int x, int y) {
    if (y < map->h - 1) {
	return map->tiles[y + 1][x];
    }
    return NO_TILE;
}

int getWestTile(Map *map, int x, int y) {
    if (x > 0) {
	return map->tiles[y][x - 1];
    }
    return NO_TILE;
}

int getNorthEastTile(Map *map, int x, int y) {
    if (y > 0) {
	return map->tiles[y - 1][x + 1];
    }
    return NO_TILE;
}

int getSouthEastTile(Map *map, int x, int y) {
    if (x < map->w - 1) {
	return map->tiles[y + 1][x + 1];
    }
    return NO_TILE;
}

int getSouthWestTile(Map *map, int x, int y) {
    if (y < map->h - 1) {
	return map->tiles[y + 1][x - 1];
    }
    return NO_TILE;
}

int getNorthWestTile(Map *map, int x, int y) {
    if (x > 0) {
	return map->tiles[y - 1][x - 1];
    }
    return NO_TILE;
}

bool isSolid(Map *map, int x, int y) {
    int tile = getTile(map, x, y);
    if (tile == TREE || tile == FOREST_TREE ||
        tile == DEEP_TREE) {
        return true;
    } else {
        return false;
    }
}
