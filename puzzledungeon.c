#include "mmm.h"
#include "raylib.h"

Map Map_createPuzzleDungeon() {
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
        int oy = (puzzlebox.y[i] - lowest_y) * 9;
        int ox = (puzzlebox.x[i] - lowest_x) * 9;
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
        if (ex < puzzlebox.x[i]) {
            map.tiles[oy + 4][ox] = DOOR;
            map.tiles[oy + 4][ox - 1] = DOOR;
        }
        if (ex > puzzlebox.x[i]) {
            map.tiles[oy + 4][ox + 8] = DOOR;
            map.tiles[oy + 4][ox + 9] = DOOR;
        }
        if (ey < puzzlebox.y[i]) {
            map.tiles[oy][ox + 4] = DOOR;
            map.tiles[oy - 1][ox + 4] = DOOR;
        }
        if (ey > puzzlebox.y[i]) {
            map.tiles[oy + 8][ox + 4] = DOOR;
            map.tiles[oy + 9][ox + 4] = DOOR;
        }
        printf("%d\n", i);
    }

    return map;
}

