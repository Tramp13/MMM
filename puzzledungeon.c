#include "mmm.h"
#include "raylib.h"
#include <stdio.h>

MapAndData Map_createPuzzleDungeon() {
    PuzzleBox puzzlebox = PuzzleBox_create();
    int lowest_x = 0, lowest_y = 0, highest_x = 0, highest_y = 0;
    for (int i = 0; i < puzzlebox.room_count; i++) {
        if (puzzlebox.x[i] < lowest_x) {
            lowest_x = puzzlebox.x[i];
        }
        if (puzzlebox.y[i] < lowest_y) {
            lowest_y = puzzlebox.y[i];
        }
        if (puzzlebox.x[i] > highest_x) {
            highest_x = puzzlebox.x[i];
        }
        if (puzzlebox.y[i] > highest_y) {
            highest_y = puzzlebox.y[i];
        }
    }

    int width = (highest_x - lowest_x) + 1;
    int height = (highest_y - lowest_y) + 1;
    width = width * 10;
    height = height * 10;
    printf("%d, %d\n\n", width, height);
    Map map = Map_create(width, height);

    for (int i = 0; i < puzzlebox.room_count; i++) {
        puzzlebox.y[i] = puzzlebox.y[i] - lowest_y;
        puzzlebox.x[i] = puzzlebox.x[i] - lowest_x;
        int oy = puzzlebox.y[i] * 9;
        int ox = puzzlebox.x[i] * 9;
        printf("%d, %d\n", ox, oy);
        for (int y = oy; y < oy + 9; y++) {
            for (int x = ox; x < ox + 9; x++) {
                if (y == oy || y == oy + 8 || x == ox || x == ox + 8) {
                    map.tiles[y][x] = STONE_WALL;
                } else {
                    map.tiles[y][x] = STONE_FLOOR;
                }
            }
        }
        int entrance = puzzlebox.entrance[i];
        int ex = puzzlebox.x[entrance];
        int ey = puzzlebox.y[entrance];
        int door_type = DOOR;
        if (puzzlebox.puzzle_type[i] == KEY_PUZZLE) {
            door_type = LOCKED_DOOR;
        } 
        if (ex < puzzlebox.x[i]) {
            map.tiles[oy + 4][ox] = door_type;
            map.tiles[oy + 4][ox - 1] = door_type;
        }
        if (ex > puzzlebox.x[i]) {
            map.tiles[oy + 4][ox + 8] = door_type;
            map.tiles[oy + 4][ox + 9] = door_type;
        }
        if (ey < puzzlebox.y[i]) {
            map.tiles[oy][ox + 4] = door_type;
            map.tiles[oy - 1][ox + 4] = door_type;
        }
        if (ey > puzzlebox.y[i]) {
            map.tiles[oy + 8][ox + 4] = door_type;
            map.tiles[oy + 9][ox + 4] = door_type;
        }
        printf("%d\n", i);
    }

    return (MapAndData) {map, puzzlebox};
}

