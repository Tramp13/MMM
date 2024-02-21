#include "raylib.h"
#include "mmm.h"
#include "perlin.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

void Map_perlinify(Map *map, int seed) {

    // Render perlin noise
    for (int y = 0; y < map->h; y++) {
	for (int x = 0; x < map->w; x++) {
	    float tile = perlin2d(x, y, 0.0075, 6, seed);
	    int tile_type;
	    if (tile < 0.5) {
		tile_type = WATER;
	    } else if (tile < 0.55) {
		tile_type = SAND;
	    } else if (tile < 0.6) {
		int chance = GetRandomValue(0, 100);
		if (chance == 0) {
		    tile_type = TREE;
		} else {
		    tile_type = GRASS;
		}
	    } else {
		int chance = GetRandomValue(0, 100);
		if (chance < 45) {
		    tile_type = FOREST_TREE;
		} else {
		    tile_type = FOREST_FLOOR;
		}
	    }
	    map->tiles[y][x] = tile_type;
	}
    }
}

void Map_enhanceForests(Map *map) {
    Map new_map = Map_create(map->w, map->h);
    //Make edges of forests mostly trees
    Map_copy(&new_map, map);
    for (int y = 0; y < map->h; y++) {
        for (int x = 0; x < map->w; x++) {
            int tile = getTile(map, x, y);
            bool is_border = (x == 0 || y == 0 || x == map->w - 1 ||
                              y == map->h - 1);
            if (!is_border && tile == FOREST_FLOOR) {
                bool make_tree = false;
                for (int dy = y - 1; dy < y + 2; dy++) {
                    for (int dx = x - 1; dx < x + 2; dx++) {
                        int delta_tile = map->tiles[dy][dx];
                        if (delta_tile == GRASS &&
                            !(x == dx && y == dy)) {
                            make_tree = true;
                        }
                    }
                }
                if (make_tree) {
                    new_map.tiles[y][x] = FOREST_TREE;
                }
            }
        }
    }
    Map_copy(map, &new_map);

    //smooth forests
    int iterations_left = 2;
    while (iterations_left > 0) {
        Map_copy(&new_map, map);
        for (int y = 0; y < map->h; y++) {
            for (int x = 0; x < map->w; x++) {
                int tile = getTile(map, x, y);
                bool is_border = (x == 0 || y == 0 ||
                                  x == map->w - 1 || y == map->h - 1);
                if (!is_border) {
                    int wall_neighbors = 0;
                    for (int dy = y - 1; dy < y + 2; dy++) {
                        for (int dx = x - 1; dx < x + 2; dx++) {
                            int delta_tile = getTile(map, dx, dy);
                            if (delta_tile == FOREST_TREE &&
                                !(x == dx && y == dy)) {
                                wall_neighbors++;
                            }
                        }
                    }
                    //printf("tile: %d, tree neighbors: %d\n", tile, wall_neighbors);
                    if (tile == FOREST_TREE && wall_neighbors < 4) {
                        new_map.tiles[y][x] = FOREST_FLOOR;
                        //printf("remove tree\n");
                    }
                    if (tile == FOREST_FLOOR && wall_neighbors >= 5) {
                        new_map.tiles[y][x] = FOREST_TREE;
                        //printf("add tree\n");
                    }
                }
            }
        }
        Map_copy(map, &new_map);
        iterations_left--;
        printf("cool\n");
    }
    
    // Hide redundant trees
    Map_copy(&new_map, map);
    for (int y = 1; y < map->h - 1; y++) {
	for (int x = 1; x < map->w - 1; x++) {
            int tile = getTile(map, x, y);
	    if (tile == FOREST_TREE) {
                int south_tile = getSouthTile(map, x, y);
                if (south_tile == FOREST_TREE) {
                    new_map.tiles[y][x] = DEEP_TREE;
                }
	    }
	}
    }
    Map_copy(map, &new_map);
    Map_free(&new_map);
}
