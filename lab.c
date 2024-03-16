#include "mmm.h"
#include "raylib.h"
#include <time.h>

int randomOddBetween(int min, int max) {
    int result = 2;
    while (result % 2 == 0) {
        result = GetRandomValue(min, max);
    }
    return result;
}

int randomEvenBetween(int min, int max) {
    int result = 1;
    while (result % 2 != 0) {
        result = GetRandomValue(min, max);
    }
    return result;
}

void Lab_extend(Map *map, int x, int y, int dx, int dy) {
    int tile = map->tiles[y + dy][x + dx];

    if (tile == STONE_WALL) {
        return;
    } else {
        map->tiles[y + dy][x + dx] = STONE_WALL;
        Lab_extend(map, x + dx, y + dy, dx, dy);
    }
}

Map Map_createLab() {
    Map map = Map_create(40, 40);

    for (int y = 0; y < map.h; y++) {
        for (int x = 0; x < map.w; x++) {
            if (x == 0 ||
                y == 0 ||
                x == map.w - 1 ||
                y == map.h - 1) {
                map.tiles[y][x] = STONE_WALL;
            } else {
                map.tiles[y][x] = STONE_FLOOR;
            }
        }
    }

    int door_x, door_y, door, i, chance;

    for (int i = 0; i < 5; i++) {
        door_x = randomOddBetween(2, map.w - 3);
        door_y = randomOddBetween(2, map.h - 3);

        map.tiles[door_y][door_x] = DOOR;

        Lab_extend(&map, door_x, door_y, 1, 0);
        Lab_extend(&map, door_x, door_y, -1, 0);

        door_x = randomEvenBetween(2, map.w - 3);
        door_y = randomEvenBetween(2, map.h - 3);

        map.tiles[door_y][door_x] = DOOR;

        Lab_extend(&map, door_x, door_y, 0, 1);
        Lab_extend(&map, door_x, door_y, 0, -1);
    }

    return map;
}
