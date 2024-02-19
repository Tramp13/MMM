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

bool isVisible(Map *map, int ax, int ay, int bx, int by) {
    Vector2 walk_point = {ax, ay};
    float angle = Vector2LineAngle((Vector2) {ax, ay},
                                   (Vector2) {bx, by});
    float anglecos = cos(angle);
    float anglesin = 0 - sin(angle);
    int max_distance = distance(ax, ay, bx, by);
    int count = 0;
    int walk_distance = 0;
    while (walk_distance < max_distance) {
        walk_point.x = (int) (ax + (count * anglecos));
        walk_point.y = (int) (ay + (count * anglesin));
        walk_distance = distance(ax, ay, walk_point.x, walk_point.y);
        int walk_tile = getTile(map, walk_point.x, walk_point.y);
        if (walk_tile == FOREST_TREE) return false;
        count++;
    }
    return true;
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
		    tile_type = FOREST_TREE;
		} else {
		    tile_type = FOREST_FLOOR;
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
                            if (delta_tile == FOREST_TREE &&
                                delta_index != tile_index) {
                                wall_neighbors++;
                            }
                        }
                    }
                    //printf("tile: %d, tree neighbors: %d\n", tile, wall_neighbors);
                    if (tile == FOREST_TREE && wall_neighbors < 4) {
                        new_tiles[tile_index] = FOREST_FLOOR;
                        //printf("remove tree\n");
                    }
                    if (tile == GRASS && wall_neighbors >= 5) {
                        new_tiles[tile_index] = FOREST_TREE;
                        //printf("add tree\n");
                    }
                }
            }
        }
        memcpy(map.tiles, new_tiles, tiles_memory_size);
        iterations_left--;
        printf("cool\n");
    }

    
    // Hide redundant trees
    memcpy(new_tiles, map.tiles, tiles_memory_size);
    for (int y = 1; y < map.h - 1; y++) {
	for (int x = 1; x < map.w - 1; x++) {
            int tile = getTile(&map, x, y);
	    int tile_index = getTileIndex(&map, x, y);
	    if (tile == FOREST_TREE) {
                int south_tile = getSouthTile(&map, x, y);
                if (south_tile == FOREST_TREE) {
                    new_tiles[tile_index] = DEEP_TREE;
                }
	    }
	}
    }
    memcpy(map.tiles, new_tiles, tiles_memory_size);
    printf("Hell yeah\n");

    /*// Soften corners
    memcpy(new_tiles, map.tiles, tiles_memory_size);
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
    memcpy(map.tiles, new_tiles, tiles_memory_size);
    printf("yep\n");
    //*/

    free(new_tiles);

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
    fov.radius = 64.0f;

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
            fov.radius = 64 * camera.zoom;
            SetShaderValue(shdr_fov, fov.radius_loc, &fov.radius,
                   SHADER_UNIFORM_FLOAT);
        }
	if (IsKeyPressed(KEY_S)) {
            camera.zoom -= 0.0625;
            fov.radius = 64 * camera.zoom;
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
                        printf("%d, %d\n", player.x, player.y);
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
                    int distance = sqrt(
                        ((player_tx - x) * (player_tx - x)) +
                        ((player_ty - y) * (player_ty - y)));
                    if (distance < 8 && player_tile == FOREST_FLOOR) {
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
                    } else {
                        DrawRectangle((x * game.tile_size), 
                                      (y * game.tile_size),
                                      game.tile_size, game.tile_size,
                                      DARKGREEN);
                    }
                        
                } else if (tile == FOREST_FLOOR) {
                    int distance = sqrt(
                        ((player_tx - x) * (player_tx - x)) +
                        ((player_ty - y) * (player_ty - y)));
                    if (distance < 8 && player_tile == FOREST_FLOOR) {
                        color = GREEN;
                    } else {
                        color = DARKGREEN;
                    }
		    DrawRectangle((x * game.tile_size), 
                                  (y * game.tile_size),
			          game.tile_size, game.tile_size,
				  color);
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

        if (player_tile == FOREST_FLOOR) {
            BeginShaderMode(shdr_fov);
                DrawRectangle(0, 0, game.screen_w, game.screen_h, BROWN);
            EndShaderMode();
        }

        EndDrawing();
    }

    // De-Initialization
    UnloadShader(shdr_fov);
    Map_free(&map);
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
