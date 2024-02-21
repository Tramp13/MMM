#include "raylib.h"
#include "raymath.h"
#include "mmm.h"
#include "perlin.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION 330
#else
    #define GLSL_VERSION 100
#endif

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

int distance(int ax, int ay, int bx, int by) {
    return sqrt(((ax - bx) * (ax - bx)) + ((ay - by) * (ay - by)));
}

typedef struct FOV {
    Vector2 position;
    float radius;

    unsigned int position_loc;
    unsigned int radius_loc;
} FOV;

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
		    tile_type = FOREST_TREE;
		} else {
		    tile_type = FOREST_FLOOR;
		}
	    }
	    map.tiles[y][x] = tile_type;
	}
    }

    //smooth forests
    int iterations_left = 2;
    Map new_map = Map_create(400, 400);
    while (iterations_left > 0) {
        Map_copy(&new_map, &map);
        for (int y = 0; y < map.h; y++) {
            for (int x = 0; x < map.w; x++) {
                int tile = getTile(&map, x, y);
                bool is_border = (x == 0 || y == 0 ||
                                  x == map.w - 1 || y == map.h - 1);
                if (!is_border) {
                    int wall_neighbors = 0;
                    for (int dy = y - 1; dy < y + 2; dy++) {
                        for (int dx = x - 1; dx < x + 2; dx++) {
                            int delta_tile = getTile(&map, dx, dy);
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
                    if (tile == GRASS && wall_neighbors >= 5) {
                        new_map.tiles[y][x] = FOREST_TREE;
                        //printf("add tree\n");
                    }
                }
            }
        }
        Map_copy(&map, &new_map);
        iterations_left--;
        printf("cool\n");
    }

    
    // Hide redundant trees
    Map_copy(&new_map, &map);
    for (int y = 1; y < map.h - 1; y++) {
	for (int x = 1; x < map.w - 1; x++) {
            int tile = getTile(&map, x, y);
	    if (tile == FOREST_TREE) {
                int south_tile = getSouthTile(&map, x, y);
                if (south_tile == FOREST_TREE) {
                    new_map.tiles[y][x] = DEEP_TREE;
                }
	    }
	}
    }
    Map_copy(&map, &new_map);
    printf("Hell yeah\n");

    Map_free(&new_map);

    InitWindow(game.screen_w, game.screen_h, "Mango Mango Monsters");

    SetTargetFPS(60);            // Set our game to run at 60 frames-per-second
    
    Entity player = Player_create(&game);
    /*player.x = game.tile_size / 2;
    player.y = game.tile_size / 2;*/

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
    int x_speed = 0;
    int y_speed = 0;
    int reg_speed = game.tile_size / 8;

    Shader shdr_fov = LoadShader(0, "fov.fs");

    FOV fov;
    fov.position = GetWorldToScreen2D((Vector2)
        {player.x, (game.screen_h - player.y)}, camera);
    fov.position.y = game.screen_h - fov.position.y;
    fov.radius = 128.0f;

    fov.position_loc = GetShaderLocation(shdr_fov, "fov.pos\0");
    fov.radius_loc = GetShaderLocation(shdr_fov, "fov.radius\0");

    SetShaderValue(shdr_fov, fov.position_loc, &fov.position.x,
                   SHADER_UNIFORM_VEC2);
    SetShaderValue(shdr_fov, fov.radius_loc, &fov.radius,
                   SHADER_UNIFORM_FLOAT);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        x_speed = 0;
        y_speed = 0;

	if (IsKeyDown(KEY_LEFT)) x_speed = -reg_speed;
	if (IsKeyDown(KEY_RIGHT)) x_speed = reg_speed;
	if (IsKeyDown(KEY_UP)) y_speed = -reg_speed;
	if (IsKeyDown(KEY_DOWN)) y_speed = reg_speed;
	if (IsKeyPressed(KEY_W)) {
            camera.zoom += 0.0625;
            fov.radius = 128 * camera.zoom;
            SetShaderValue(shdr_fov, fov.radius_loc, &fov.radius,
                   SHADER_UNIFORM_FLOAT);
        }
	if (IsKeyPressed(KEY_S)) {
            camera.zoom -= 0.0625;
            fov.radius = 128 * camera.zoom;
            SetShaderValue(shdr_fov, fov.radius_loc, &fov.radius,
                   SHADER_UNIFORM_FLOAT);
        }
	if (IsKeyPressed(KEY_A)) reg_speed -= 2;
	if (IsKeyPressed(KEY_D)) reg_speed += 2;

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

        fov.position = GetWorldToScreen2D((Vector2)
            {player.x, player.y}, camera);
        fov.position.y = game.screen_h - fov.position.y;
        SetShaderValue(shdr_fov, fov.position_loc, &fov.position.x,
                       SHADER_UNIFORM_VEC2);

        player.x += x_speed;
        player.y += y_speed;
        
        int player_tx = (int) player.x / game.tile_size;
        int player_ty = (int) player.y / game.tile_size;
        int player_tile = getTile(&map, player_tx, player_ty);

        for (int dy = player_ty - 1; dy < player_ty + 2; dy++) {
            for (int dx = player_tx - 1; dx < player_tx + 2; dx++) {
                bool is_solid = isSolid(&map, dx, dy);
                if (is_solid) {
                    Rectangle tile_rect;
                    tile_rect.x = dx * game.tile_size;
                    tile_rect.y = dy * game.tile_size;
                    tile_rect.width = game.tile_size;
                    tile_rect.height = game.tile_size;
                    Vector2 player_center = (Vector2) {player.x, player.y};
                    bool collides = CheckCollisionCircleRec(player_center,
                                        player.radius, tile_rect);
                    while (collides) {
                        int tile_center_x =
                            (int) tile_rect.x + (tile_rect.width / 2);
                        int tile_center_y =
                            (int) tile_rect.y + (tile_rect.height / 2);
                        int x_distance = abs(player.x - tile_center_x);
                        int y_distance = abs(player.y - tile_center_y);
                        if (y_distance > x_distance ||
                            y_distance == x_distance) {
                            if (y_speed < 0) {
                                player.y += 1;
                            }
                            if (y_speed > 0) {
                                player.y -= 1;
                            }
                            if (y_speed == 0) {
                                if (player.y < tile_center_y) {
                                    player.y -= 1;
                                }
                                if (player.y > tile_center_y) {
                                    player.y += 1;
                                }
                            }
                        }
                        if (x_distance > y_distance ||
                            x_distance == y_distance) {
                            if (x_speed < 0) {
                                player.x += 1;
                            }
                            if (x_speed > 0) {
                                player.x -= 1;
                            }
                            if (x_speed == 0) {
                                if (player.x < tile_center_x) {
                                    player.x -= 1;
                                }
                                if (player.x > tile_center_x) {
                                    player.x += 1;
                                }
                            }
                        }
                        player_center = (Vector2) {player.x, player.y};
                        collides = CheckCollisionCircleRec(player_center,
                                        player.radius, tile_rect);
                    }
                }
            }
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
        int post_renders[2000];
        int post_render_count = 0;

	for (int y = top; y <= bottom; y++) {
	   for (int x = left; x <= right; x++) {
		Color color;
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
		    /*Vector2 a, b, c;
		    a = (Vector2){x * game.tile_size,
			 (y * game.tile_size) + game.tile_size};
		    b = (Vector2){(x * game.tile_size) + game.tile_size,
			 (y * game.tile_size) + game.tile_size};
		    c = (Vector2){(x * game.tile_size) + (game.tile_size / 2),
			 y * game.tile_size};*/
		    DrawRectangle(x * game.tile_size, y * game.tile_size,
			          game.tile_size, game.tile_size, GREEN);
		    DrawRectangle((x * game.tile_size), 
                                  (y * game.tile_size),
			          game.tile_size, game.tile_size / 2,
				  DARKGREEN);
		    DrawRectangle((x * game.tile_size) + game.tile_size / 4,
				  (y * game.tile_size) + game.tile_size / 2,
			          game.tile_size / 2, game.tile_size / 2,
				  BROWN);
		    //DrawTriangle(a, b, c, DARKGREEN);
		} else if (tile == FOREST_TREE) {
                    DrawRectangle(x * game.tile_size, y * game.tile_size,
                                  game.tile_size, game.tile_size, GREEN);
                    DrawRectangle((x * game.tile_size), 
                                  (y * game.tile_size),
                                  game.tile_size, game.tile_size / 2,
                                  DARKGREEN);
                    DrawRectangle((x * game.tile_size) + game.tile_size / 4,
                                  (y * game.tile_size) + game.tile_size / 2,
                                  game.tile_size / 2, game.tile_size / 2,
                                  BROWN);
                    post_renders[(post_render_count * 2)] = x;
                    post_renders[(post_render_count * 2) + 1] = y;
                    post_render_count++;
                } else if (tile == FOREST_FLOOR) {
		    DrawRectangle((x * game.tile_size), 
                                  (y * game.tile_size),
			          game.tile_size, game.tile_size,
				  GREEN);
                    post_renders[(post_render_count * 2)] = x;
                    post_renders[(post_render_count * 2) + 1] = y;
                    post_render_count++;
                } else if (tile == DEEP_TREE) {
                    DrawRectangle(x * game.tile_size, y * game.tile_size,
                                  game.tile_size, game.tile_size,
                                  DARKGREEN);
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

	DrawCircle(player.x,
                   player.y, player.radius,
                   player.color);
                
	EndMode2D();

        //if (player_tile == FOREST_FLOOR) {
            BeginShaderMode(shdr_fov);
                for (int i = 0; i < post_render_count; i++) {
                    int x = post_renders[i * 2] * game.tile_size;
                    int y = post_renders[(i * 2) + 1] * game.tile_size;
                    Vector2 pos = GetWorldToScreen2D((Vector2) {x, y}, camera);
                    DrawRectangle(pos.x,
                                  pos.y, game.tile_size,
                                  game.tile_size, DARKGREEN);
                }
            EndShaderMode();
        //}

        EndDrawing();
    }

    // De-Initialization
    UnloadShader(shdr_fov);
    Map_free(&map);
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
