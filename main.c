#include "raylib.h"
#include "raymath.h"
#include "mmm.h"
#include "perlin.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void drawSlantTileNW(int tile_x, int tile_y, int tile_size, Color color) {
    Vector2 a, b, c;
    a = (Vector2){tile_x * tile_size,
		  (tile_y * tile_size) + tile_size};
    b = (Vector2){(tile_x * tile_size) + tile_size,
		  (tile_y * tile_size)};
    c = (Vector2){(tile_x * tile_size),
		  (tile_y * tile_size)};
    DrawTriangle(a, b, c, color);
}

void drawSlantTileNE(int tile_x, int tile_y, int tile_size, Color color) {
	Vector2 a, b, c;
	a = (Vector2){(tile_x * tile_size) + tile_size,
		      (tile_y * tile_size)};
	b = (Vector2){(tile_x * tile_size),
		      (tile_y * tile_size)};
	c = (Vector2){(tile_x * tile_size) + tile_size,
		      (tile_y * tile_size) + tile_size};
	DrawTriangle(a, b, c, color);
}

void drawSlantTileSW(int tile_x, int tile_y, int tile_size, Color color) {
	Vector2 a, b, c;
	a = (Vector2){(tile_x * tile_size),
		      (tile_y * tile_size) + tile_size};
	b = (Vector2){(tile_x * tile_size) + tile_size,
		      (tile_y * tile_size) + tile_size};
	c = (Vector2){(tile_x * tile_size),
		      (tile_y * tile_size)};
	DrawTriangle(a, b, c, color);
}

void drawSlantTileSE(int tile_x, int tile_y, int tile_size, Color color) {
	Vector2 a, b, c;
	a = (Vector2){(tile_x * tile_size),
		      (tile_y * tile_size) + tile_size};
	b = (Vector2){(tile_x * tile_size) + tile_size,
		      (tile_y * tile_size) + tile_size};
	c = (Vector2){(tile_x * tile_size) + tile_size,
		      (tile_y * tile_size)};
	DrawTriangle(a, b, c, color);
}


// Program main entry point
int main(void)
{
    // Initialization
    Game game;
    game.screen_w = 800;
    game.screen_h = 450;
    game.scale = 1;
    game.tile_size = 32;

    Map map = Map_create(400, 400);

    int seed = (int) time(NULL);
    SetRandomSeed(seed);

    // Render perlin noise
    for (int y = 0; y < map.h; y++) {
	for (int x = 0; x < map.w; x++) {
	    float tile = perlin2d(x, y, 0.0075, 6, seed);
	    int tile_index = getTileIndex(&map, x, y);
	    int tile_type;
	    if (tile < 0.55) {
		tile_type = WATER;
	    } else if (tile < 0.6) {
		tile_type = SAND;
	    } else if (tile < 0.65) {
		int chance = GetRandomValue(0, 100);
		if (chance == 0) {
		    tile_type = TREE;
		} else {
		    tile_type = GRASS;
		}
	    } else {
		int chance = GetRandomValue(0, 100);
		if (chance < 45) {
		    tile_type = TREE;
		} else {
		    tile_type = GRASS;
		}
	    }
	    map.tiles[tile_index] = tile_type;
	}
    }

    //smooth forests
    size_t tiles_memory_size = sizeof(int) * (map.w * map.h);
    int *new_tiles = malloc(tiles_memory_size);
    //int new_tiles[map.w * map.h];
    int iterations_left = 2;
    while (iterations_left > 0) {
        memcpy(new_tiles, map.tiles, tiles_memory_size);
        for (int y = 0; y < map.h; y++) {
            for (int x = 0; x < map.w; x++) {
                int tile = getTile(&map, x, y);
                int tile_index = getTileIndex(&map, x, y);
                bool is_border = (x == 0 || y == 0 ||
                                  x == map.w - 1 || y == map.h - 1);
                if (!is_border) {
                    int wall_neighbors = 0;
                    for (int dy = y - 1; dy < y + 2; dy++) {
                        for (int dx = x - 1; dx < x + 2; dx++) {
                            int delta_tile = getTile(&map, dx, dy);
                            int delta_index = getTileIndex(&map, dx, dy);
                            if (delta_tile == TREE &&
                                delta_index != tile_index) {
                                wall_neighbors++;
                            }
                        }
                    }
                    //printf("tile: %d, tree neighbors: %d\n", tile, wall_neighbors);
                    if (tile == TREE && wall_neighbors < 4) {
                        new_tiles[tile_index] = GRASS;
                        //printf("remove tree\n");
                    }
                    if (tile == GRASS && wall_neighbors >= 5) {
                        new_tiles[tile_index] = TREE;
                        //printf("add tree\n");
                    }
                }
            }
        }
        memcpy(map.tiles, new_tiles, tiles_memory_size);
        iterations_left--;
        printf("cool\n");
    }

    
    /*// Hide redundant trees
    memcpy(new_tiles, map.tiles, sizeof(map.tiles));
    for (int y = 1; y < map.h - 1; y++) {
	for (int x = 1; x < map.w - 1; x++) {
            int tile = getTile(&map, x, y);
	    int tile_index = getTileIndex(&map, x, y);
	    if (tile == TREE) {
                int north_tile = getNorthTile(&map, x, y);
                int east_tile = getEastTile(&map, x, y);
                int south_tile = getSouthTile(&map, x, y);
                int west_tile = getWestTile(&map, x, y);;
                if (north_tile == TREE && east_tile == TREE &&
                    south_tile == TREE && west_tile == TREE) {
                    new_tiles[tile_index] = DEEP_TREE;
                }
	    }
	}
    }
    memcpy(map.tiles, new_tiles, sizeof(map.tiles));
    printf("Hell yeah\n");

    // Soften corners
    memcpy(new_tiles, map.tiles, sizeof(map.tiles));
    for (int y = 0; y < map.h; y++) {
	for (int x = 0; x < map.w; x++) {
	    int tile_index = getTileIndex(&map, x, y);
	    int tile = getTile(&map, x, y);
            int north_tile = getNorthTile(&map, x, y);
            int east_tile = getEastTile(&map, x, y);
            int south_tile = getSouthTile(&map, x, y);
            int west_tile = getWestTile(&map, x, y);;
	    if (tile == SAND) {
		if (south_tile == WATER) {
		    if (east_tile == WATER) {
			new_tiles[tile_index] = NW_SAND_SE_WATER;
		    }
		    if (west_tile == WATER) {
			new_tiles[tile_index] = NE_SAND_SW_WATER;
		    }
		}
		if (north_tile == WATER) {
		    if (east_tile == WATER) {
			new_tiles[tile_index] = SW_SAND_NE_WATER;
		    }
		    if (west_tile == WATER) {
			new_tiles[tile_index] = SE_SAND_NW_WATER;
		    }
		}
	    }
	    if (tile == GRASS) {
		if (south_tile == SAND) {
		    if (east_tile == SAND) {
			new_tiles[tile_index] = NW_GRASS_SE_SAND;
		    }
		    if (west_tile == SAND) {
			new_tiles[tile_index] = NE_GRASS_SW_SAND;
		    }
		}
		if (north_tile == SAND) {
		    if (east_tile == SAND) {
			new_tiles[tile_index] = SW_GRASS_NE_SAND;
		    }
		    if (west_tile == SAND) {
			new_tiles[tile_index] = SE_GRASS_NW_SAND;
		    }
		}
	    }
	}
    }
    memcpy(map.tiles, new_tiles, sizeof(map.tiles));
    printf("yep\n");
    //*/

    free(new_tiles);

    InitWindow(game.screen_w, game.screen_h, "Mango Mango Monsters");

    SetTargetFPS(60);            // Set our game to run at 60 frames-per-second
    
    Entity player = Player_create(&game);
    player.x = 0;
    player.y = 0;

    //find free space for player
    bool player_pos_found = false;
    while (!player_pos_found) {
	int x = GetRandomValue(0, map.w - 1);
	int y = GetRandomValue(0, map.h - 1);
        int tile = getTile(&map, x, y);
	if (tile == GRASS) {
	    player.x = x * game.tile_size;
	    player.y = y * game.tile_size;
	    player_pos_found = true;
	}
    }

    Camera2D camera = { 0 };
    camera.target = (Vector2){player.x, player.y};
    camera.offset = (Vector2){game.screen_w / 2, game.screen_h / 2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    int speed = 2;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        // TODO: Update your variables here
	if (IsKeyDown(KEY_LEFT)) player.x -= speed;
	if (IsKeyDown(KEY_RIGHT)) player.x += speed;
	if (IsKeyDown(KEY_UP)) player.y -= speed;
	if (IsKeyDown(KEY_DOWN)) player.y += speed;
	if (IsKeyPressed(KEY_W)) camera.zoom += 0.0625;
	if (IsKeyPressed(KEY_S)) camera.zoom -= 0.0625;
	if (IsKeyPressed(KEY_A)) speed -= 2;
	if (IsKeyPressed(KEY_D)) speed += 2;

	camera.target = (Vector2){player.x, player.y};
        camera.offset = (Vector2){game.screen_w / 2, game.screen_h / 2};
	int player_x_scaled = player.x * camera.zoom;
	int player_y_scaled = player.y * camera.zoom;
	if (player_x_scaled < (game.screen_w / 2)) {
	    camera.offset.x = player_x_scaled;
	}
	if (player_y_scaled < (game.screen_h / 2)) {
	    camera.offset.y = player_y_scaled;
	}
	int right_limit = ((map.w * game.tile_size) + game.tile_size);
	int bottom_limit = ((map.h * game.tile_size) + game.tile_size);
	int player_distance_from_right = (right_limit - player.x) * camera.zoom;
	int player_distance_from_bottom = (bottom_limit - player.y) * camera.zoom;
	if (player_distance_from_right < (game.screen_w / 2)) {
	    camera.offset.x = game.screen_w - player_distance_from_right;
	}
	if (player_distance_from_bottom < (game.screen_h / 2)) {
	    camera.offset.y = game.screen_h - player_distance_from_bottom;
	}

        // Draw
        BeginDrawing();

	ClearBackground(RAYWHITE);

	BeginMode2D(camera);

	int tile_size = game.tile_size * camera.zoom;
	Vector2 top_left = GetScreenToWorld2D((Vector2){0, 0}, camera);
	int top = top_left.y / game.tile_size;
	int left = top_left.x / game.tile_size;
	Vector2 bottom_right = GetScreenToWorld2D(
	    (Vector2){game.screen_w - 1, 
	              game.screen_h - 1},
	    camera);
	int bottom = bottom_right.y / game.tile_size;
	int right = bottom_right.x / game.tile_size;

        Vector2 tree_points[1500];
        int tree_point_count = 0;

	for (int y = top; y <= bottom; y++) {
	   for (int x = left; x <= right; x++) {
		Color color;
		int tile_index = getTileIndex(&map, x, y);
		int tile = getTile(&map, x, y);
		if (tile == WATER) {
		    color = BLUE;
		} else if (tile == SAND) {
		    color = WHITE;
		} else if (tile == GRASS) {
		    color = GREEN;
		} else {
		    color = BLACK;
		}
		if (tile == TREE) {
                    int north_tile = getNorthTile(&map, x, y);
                    int east_tile = getEastTile(&map, x, y);
                    int south_tile = getSouthTile(&map, x, y);
                    int west_tile = getWestTile(&map, x, y);
                    /*if ((north_tile != TREE &&
                        north_tile != DEEP_TREE) ||
                        (west_tile != TREE &&
                        west_tile != DEEP_TREE)) {
                        tree_points[tree_point_count] = (Vector2) {
                            x * game.tile_size,
                            y * game.tile_size
                        };
                        tree_point_count++;
                    }
                    if ((north_tile != TREE && north_tile != DEEP_TREE) ||
                        (east_tile != TREE && east_tile != DEEP_TREE)) {
                        tree_points[tree_point_count] = (Vector2) {
                            (x * game.tile_size) + game.tile_size,
                            y * game.tile_size
                        };
                        tree_point_count++;
                    }
                    if ((south_tile != TREE && south_tile != DEEP_TREE) ||
                        (west_tile != TREE && west_tile != DEEP_TREE)) {
                        tree_points[tree_point_count] = (Vector2) {
                            x * game.tile_size,
                            (y * game.tile_size) + game.tile_size
                        };
                        tree_point_count++;
                    }
                    if ((south_tile != TREE && south_tile != DEEP_TREE) ||
                        (east_tile != TREE && east_tile != DEEP_TREE)) {
                        tree_points[tree_point_count] = (Vector2) {
                            (x * game.tile_size) + game.tile_size,
                            (y * game.tile_size) + game.tile_size
                        };
                        tree_point_count++;
                    }*/
		    Vector2 a, b, c;
		    a = (Vector2){x * game.tile_size,
			 (y * game.tile_size) + game.tile_size};
		    b = (Vector2){(x * game.tile_size) + game.tile_size,
			 (y * game.tile_size) + game.tile_size};
		    c = (Vector2){(x * game.tile_size) + (game.tile_size / 2),
			 y * game.tile_size};
		    DrawRectangle(x * game.tile_size, y * game.tile_size,
			          game.tile_size, game.tile_size, GREEN);
		    DrawRectangle((x * game.tile_size) + 2, 
                                  (y * game.tile_size) + 2,
			          game.tile_size - 4, game.tile_size / 2,
				  DARKGREEN);
		    DrawRectangle((x * game.tile_size) + game.tile_size / 4,
				  (y * game.tile_size) + game.tile_size / 2,
			          game.tile_size / 2, game.tile_size / 2,
				  BROWN);
		    //DrawTriangle(a, b, c, DARKGREEN);
		} else if (tile == NW_SAND_SE_WATER) {
		    drawSlantTileNW(x, y, game.tile_size, WHITE);
		    drawSlantTileSE(x, y, game.tile_size, BLUE);
		} else if (tile == NE_SAND_SW_WATER) {
		    drawSlantTileNE(x, y, game.tile_size, WHITE);
		    drawSlantTileSW(x, y, game.tile_size, BLUE);
		} else if (tile == SW_SAND_NE_WATER) {
		    drawSlantTileSW(x, y, game.tile_size, WHITE);
		    drawSlantTileNE(x, y, game.tile_size, BLUE);
		} else if (tile == SE_SAND_NW_WATER) {
		    drawSlantTileSE(x, y, game.tile_size, WHITE);
		    drawSlantTileNW(x, y, game.tile_size, BLUE);
		} else if (tile == NW_GRASS_SE_SAND) {
		    drawSlantTileNW(x, y, game.tile_size, GREEN);
		    drawSlantTileSE(x, y, game.tile_size, WHITE);
		} else if (tile == NE_GRASS_SW_SAND) {
		    drawSlantTileNE(x, y, game.tile_size, GREEN);
		    drawSlantTileSW(x, y, game.tile_size, WHITE);
		} else if (tile == SW_GRASS_NE_SAND) {
		    drawSlantTileSW(x, y, game.tile_size, GREEN);
		    drawSlantTileNE(x, y, game.tile_size, WHITE);
		} else if (tile == SE_GRASS_NW_SAND) {
		    drawSlantTileSE(x, y, game.tile_size, GREEN);
		    drawSlantTileNW(x, y, game.tile_size, WHITE);
		} else {
		    DrawRectangle(x * game.tile_size, y * game.tile_size,
			          game.tile_size, game.tile_size, color);
		}
	    }
	}

	DrawCircle(player.x, player.y, player.radius, player.color);
        /*Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), camera);
        DrawCircle(mouse.x, mouse.y, player.radius, player.color);
        float angle = Vector2LineAngle((Vector2) {player.x, player.y}, mouse);
        float anglecos = cos(angle);
        float anglesin = 0 - sin(angle);
        int circle_x = player.x + (20 * anglecos);
        int circle_y = player.y + (20 * anglesin);
        for (int i = 0; i < 5; i++) {
            int circle_x = player.x + ((i * 10) * anglecos);
            int circle_y = player.y + ((i * 10) * anglesin);
            DrawCircle(circle_x, circle_y, player.radius / 2, player.color);
        }*/

        /*for (int i = 0; i < tree_point_count; i++) {
            Vector2 point = tree_points[i];
            Vector2 line = {player.x, player.y};
            float angle = Vector2LineAngle((Vector2) {
                player.x, 
                player.y}, point);
            float anglecos = cos(angle);
            float anglesin = 0 - sin(angle);
            int count = 0;
            while (abs(line.x - point.x) > 5 || abs(line.y - point.y) > 5) {
                line.x = (int) (player.x + (count * anglecos));
                line.y = (int) (player.y + (count * anglesin));
                //printf("%d, %d\n");
                DrawLine(player.x, player.y, line.x, line.y, RED);
                count++;
            }
        }*/

	EndMode2D();

        EndDrawing();
    }

    // De-Initialization
    Map_free(&map);
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
