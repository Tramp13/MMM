#include "raylib.h"
#include "mmm.h"
#include "perlin.h"
#include <stdio.h>
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

int getNorthTile(int x, int y, int tiles[], int map_w, int map_h) {
    if (y > 0) {
	return tiles[((y - 1) * map_w) + x];
    }
    return 0;
}

int getEastTile(int x, int y, int tiles[], int map_w, int map_h) {
    if (x < map_w - 1) {
	return tiles[(y * map_w) + (x + 1)];
    }
    return 0;
}

int getSouthTile(int x, int y, int tiles[], int map_w, int map_h) {
    if (y < map_h - 1) {
	return tiles[((y + 1) * map_w) + x];
    }
    return 0;
}

int getWestTile(int x, int y, int tiles[], int map_w, int map_h) {
    if (x > 0) {
	return tiles[(y * map_w) + (x - 1)];
    }
    return 0;
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

    int tiles[map.w * map.h];
    // Render perlin noise
    for (int y = 0; y < map.h; y++) {
	for (int x = 0; x < map.w; x++) {
	    float tile = perlin2d(x, y, 0.0075, 6, seed);
	    int tile_index = (y * map.w) + x;
	    int tile_type;
	    if (tile < 0.55) {
		tile_type = WATER;
	    } else if (tile < 0.6) {
		tile_type = SAND;
	    } else if (tile < 0.8) {
		int chance = GetRandomValue(0, 100);
		if (chance == 0) {
		    tile_type = TREE;
		} else {
		    tile_type = GRASS;
		}
	    } else {
		int chance = GetRandomValue(0, 2);
		printf("%d\n", chance);
		if (chance == 0) {
		    tile_type = TREE;
		} else {
		    tile_type = GRASS;
		}
	    }
	    tiles[tile_index] = tile_type;
	}
    }

    // Soften corners
    int new_tiles[map.w * map.h];
    memcpy(new_tiles, tiles, sizeof(tiles));
    for (int y = 0; y < map.h; y++) {
	for (int x = 0; x < map.w; x++) {
	    int tile_index = (y * map.w) + x;
	    int tile = tiles[tile_index];
	    if (tile == SAND) {
		if (getSouthTile(x, y, tiles, map.w, map.h) == WATER) {
		    if (getEastTile(x, y, tiles, map.w, map.h) == WATER) {
			new_tiles[tile_index] = NW_SAND_SE_WATER;
		    }
		    if (getWestTile(x, y, tiles, map.w, map.h) == WATER) {
			new_tiles[tile_index] = NE_SAND_SW_WATER;
		    }
		}
		if (getNorthTile(x, y, tiles, map.w, map.h) == WATER) {
		    if (getEastTile(x, y, tiles, map.w, map.h) == WATER) {
			new_tiles[tile_index] = SW_SAND_NE_WATER;
		    }
		    if (getWestTile(x, y, tiles, map.w, map.h) == WATER) {
			new_tiles[tile_index] = SE_SAND_NW_WATER;
		    }
		}
	    }
	    if (tile == GRASS) {
		if (getSouthTile(x, y, tiles, map.w, map.h) == SAND) {
		    if (getEastTile(x, y, tiles, map.w, map.h) == SAND) {
			new_tiles[tile_index] = NW_GRASS_SE_SAND;
		    }
		    if (getWestTile(x, y, tiles, map.w, map.h) == SAND) {
			new_tiles[tile_index] = NE_GRASS_SW_SAND;
		    }
		}
		if (getNorthTile(x, y, tiles, map.w, map.h) == SAND) {
		    if (getEastTile(x, y, tiles, map.w, map.h) == SAND) {
			new_tiles[tile_index] = SW_GRASS_NE_SAND;
		    }
		    if (getWestTile(x, y, tiles, map.w, map.h) == SAND) {
			new_tiles[tile_index] = SE_GRASS_NW_SAND;
		    }
		}
	    }
	}
    }
    memcpy(tiles, new_tiles, sizeof(tiles));

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
	if (tiles[(y * map.w) + x] == GRASS) {
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

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        // TODO: Update your variables here
	if (IsKeyDown(KEY_LEFT)) player.x -= 2;
	if (IsKeyDown(KEY_RIGHT)) player.x += 2;
	if (IsKeyDown(KEY_UP)) player.y -= 2;
	if (IsKeyDown(KEY_DOWN)) player.y += 2;
	if (IsKeyPressed(KEY_W)) camera.zoom += 0.0625;
	if (IsKeyPressed(KEY_S)) camera.zoom -= 0.0625;

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

	for (int y = top; y <= bottom; y++) {
	   for (int x = left; x <= right; x++) {
		Color color;
		int tile_index = (y * map.w) + x;
		int tile_type = tiles[tile_index];
		if (tile_type == WATER) {
		    color = BLUE;
		} else if (tile_type == SAND) {
		    color = WHITE;
		} else if (tile_type == GRASS) {
		    color = GREEN;
		} else {
		    color = GREEN;
		}
		if (tile_type == TREE) {
		    Vector2 a, b, c;
		    a = (Vector2){x * game.tile_size,
			 (y * game.tile_size) + game.tile_size};
		    b = (Vector2){(x * game.tile_size) + game.tile_size,
			 (y * game.tile_size) + game.tile_size};
		    c = (Vector2){(x * game.tile_size) + (game.tile_size / 2),
			 y * game.tile_size};
		    DrawRectangle(x * game.tile_size, y * game.tile_size,
			          game.tile_size, game.tile_size, GREEN);
		    DrawRectangle(x * game.tile_size, y * game.tile_size,
			          game.tile_size, game.tile_size / 2,
				  DARKGREEN);
		    DrawRectangle((x * game.tile_size) + game.tile_size / 4,
				  (y * game.tile_size) + game.tile_size / 2,
			          game.tile_size / 2, game.tile_size / 2,
				  BROWN);
		    //DrawTriangle(a, b, c, DARKGREEN);
		} else if (tile_type == NW_SAND_SE_WATER) {
		    drawSlantTileNW(x, y, game.tile_size, WHITE);
		    drawSlantTileSE(x, y, game.tile_size, BLUE);
		} else if (tile_type == NE_SAND_SW_WATER) {
		    drawSlantTileNE(x, y, game.tile_size, WHITE);
		    drawSlantTileSW(x, y, game.tile_size, BLUE);
		} else if (tile_type == SW_SAND_NE_WATER) {
		    drawSlantTileSW(x, y, game.tile_size, WHITE);
		    drawSlantTileNE(x, y, game.tile_size, BLUE);
		} else if (tile_type == SE_SAND_NW_WATER) {
		    drawSlantTileSE(x, y, game.tile_size, WHITE);
		    drawSlantTileNW(x, y, game.tile_size, BLUE);
		} else if (tile_type == NW_GRASS_SE_SAND) {
		    drawSlantTileNW(x, y, game.tile_size, GREEN);
		    drawSlantTileSE(x, y, game.tile_size, WHITE);
		} else if (tile_type == NE_GRASS_SW_SAND) {
		    drawSlantTileNE(x, y, game.tile_size, GREEN);
		    drawSlantTileSW(x, y, game.tile_size, WHITE);
		} else if (tile_type == SW_GRASS_NE_SAND) {
		    drawSlantTileSW(x, y, game.tile_size, GREEN);
		    drawSlantTileNE(x, y, game.tile_size, WHITE);
		} else if (tile_type == SE_GRASS_NW_SAND) {
		    drawSlantTileSE(x, y, game.tile_size, GREEN);
		    drawSlantTileNW(x, y, game.tile_size, WHITE);
		} else {
		    DrawRectangle(x * game.tile_size, y * game.tile_size,
			          game.tile_size, game.tile_size, color);
		}
	    }
	}

	DrawCircle(player.x, player.y, player.radius, player.color);

	EndMode2D();

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
