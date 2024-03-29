#include "raylib.h"
#include "raymath.h"
#include "mmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION 330
#else
    #define GLSL_VERSION 100
#endif

int distance(int ax, int ay, int bx, int by) {
    return sqrt(((ax - bx) * (ax - bx)) + ((ay - by) * (ay - by)));
}

typedef struct FOV {
    Vector2 position;
    float radius;

    unsigned int position_loc;
    unsigned int radius_loc;
} FOV;

int inventory_has(int item_type, Entity *inventory) {
    for (int i = 0; i < 20; i++) {
        if (inventory[i].type == item_type) {
            return i;
        }
    }
    return -1;
}

void inventory_add(Entity item, Entity *inventory) {
    for (int i = 0; i < 20; i++) {
        if (inventory[i].type == NO_ITEM) {
            printf("Added");
            inventory[i] = item;
            return;
        }
    }
    return;
}

// Program main entry point
int main(void)
{

    int seed = (int) time(NULL);
    SetRandomSeed(seed);

    // Initialization
    Game game;
    game.screen_w = 800;
    game.screen_h = 450;
    game.scale = 1;
    game.tile_size = 36;

    InitWindow(game.screen_w, game.screen_h, "Mango Mango Monsters");

    SetTargetFPS(60);            // Set our game to run at 60 frames-per-second
    
    Map map = Map_create(400, 400);
    PuzzleBox map_data;
    Entity items[20];
    for (int i = 0; i < 20; i++) {
        items[i].type = NO_ITEM;
    }
    Entity inventory[20];
    for (int i = 0; i < 20; i++) {
        inventory[i].type = NO_ITEM;
    }

    Map_perlinify(&map, seed);

    Map_enhanceForests(&map);

    Entity player = Player_create(&game);
    /*player.x = game.tile_size / 2;
    player.y = game.tile_size / 2;*/

    //find free space for player
    Vector2 player_pos = Map_findRandomTile(&map, GRASS);
    player.x = player_pos.x * game.tile_size;
    player.y = player_pos.y * game.tile_size;
    map.tiles[(int) player_pos.y][(int) player_pos.x] = STAIRS;

    Camera2D camera = { 0 };
    camera.target = (Vector2){player.x, player.y};
    camera.offset = (Vector2){game.screen_w / 2, game.screen_h / 2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    int x_speed = 0;
    int y_speed = 0;
    int reg_speed = game.tile_size / 8;
    

    Shader shdr_fov = LoadShader(0, TextFormat("fov%i.fs", GLSL_VERSION));
    FOV fov;
    fov.position = GetWorldToScreen2D((Vector2)
        {player.x, (game.screen_h - player.y)}, camera);
    fov.position.y = game.screen_h - fov.position.y;
    float default_fov_radius = 128.0f * 2;
    fov.radius = default_fov_radius;

    fov.position_loc = GetShaderLocation(shdr_fov, "fov.pos\0");
    fov.radius_loc = GetShaderLocation(shdr_fov, "fov.radius\0");

    SetShaderValue(shdr_fov, fov.position_loc, &fov.position.x,
                   SHADER_UNIFORM_VEC2);
    SetShaderValue(shdr_fov, fov.radius_loc, &fov.radius,
                   SHADER_UNIFORM_FLOAT);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        int player_tx = (int) player.x / game.tile_size;
        int player_ty = (int) player.y / game.tile_size;
        int player_tile = getTile(&map, player_tx, player_ty);
        
        x_speed = 0;
        y_speed = 0;

	if (IsKeyDown(KEY_LEFT)) x_speed = -reg_speed;
	if (IsKeyDown(KEY_RIGHT)) x_speed = reg_speed;
	if (IsKeyDown(KEY_UP)) y_speed = -reg_speed;
	if (IsKeyDown(KEY_DOWN)) y_speed = reg_speed;
        if (IsKeyPressed(KEY_F) && player_tile == STAIRS) {
            Map_free(&map);
            MapAndData map_and_data = Map_createPuzzleDungeon();
            map = map_and_data.map;
            map_data = map_and_data.puzzlebox;
            player.x = (map_data.x[0] * (game.tile_size * 9)) +
                        (game.tile_size * 2);
            player.y = (map_data.y[0] * (game.tile_size * 9)) + 
                        (game.tile_size * 2);
            for (int i = 0; i < map_data.room_count; i++) {
                if (map_data.puzzle_type[i] == KEY_PUZZLE) {
                    Entity key;
                    key.x = (map_data.x[i] * (game.tile_size * 9)) +
                             game.tile_size;
                    key.y = (map_data.y[i] * (game.tile_size * 9)) +
                             game.tile_size;
                    printf("key: %d %d\n", key.x, key.y);
                    key.radius = game.tile_size / 4;
                    key.color = YELLOW;
                    key.type = KEY;
                    for (int y = 0; y < 20; y++) {
                        if (items[y].type == NO_ITEM) {
                            items[y] = key;
                        }
                    }
                }
            }
        }

	if (IsKeyPressed(KEY_W)) {
            camera.zoom += 0.0625;
            fov.radius = default_fov_radius * camera.zoom;
            SetShaderValue(shdr_fov, fov.radius_loc, &fov.radius,
                   SHADER_UNIFORM_FLOAT);
        }
	if (IsKeyPressed(KEY_S)) {
            camera.zoom -= 0.0625;
            fov.radius = default_fov_radius * camera.zoom;
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
        
        player_tx = (int) player.x / game.tile_size;
        player_ty = (int) player.y / game.tile_size;
        player_tile = getTile(&map, player_tx, player_ty);

        for (int dy = player_ty - 1; dy < player_ty + 2; dy++) {
            for (int dx = player_tx - 1; dx < player_tx + 2; dx++) {
                if (map.tiles[dy][dx] == LOCKED_DOOR) {
                    Rectangle tile_rect;
                    tile_rect.x = dx * game.tile_size;
                    tile_rect.y = dy * game.tile_size;
                    tile_rect.width = game.tile_size;
                    tile_rect.height = game.tile_size;
                    Vector2 player_center = (Vector2) {player.x, player.y};
                    bool collides = CheckCollisionCircleRec(player_center,
                                        player.radius, tile_rect);
                    if (collides) {
                        int key_index = inventory_has(KEY, inventory);
                        if (key_index != -1) {
                            map.tiles[dy][dx] = DOOR;
                            inventory[key_index].type = NO_ITEM;
                        }
                    }
                }
        
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

        for (int i = 0; i < 20; i++) {
            if (items[i].type != NO_ITEM) {
                Vector2 player_center = (Vector2) {player.x, player.y};
                Vector2 item_center = (Vector2) {items[i].x, items[i].y};
                int collides = CheckCollisionCircles(player_center,
                                                     player.radius,
                                                     item_center,
                                                     items[i].radius);
                if (collides) {
                    inventory_add(items[i], inventory);
                    inventory_add(items[i], inventory);
                    items[i].type = NO_ITEM;
                    printf("1");
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
		    DrawRectangle((x * game.tile_size),
				  (y * game.tile_size),
			          game.tile_size, game.tile_size,
				  BROWN);
		    DrawRectangle((x * game.tile_size), 
                                  (y * game.tile_size),
			          game.tile_size, game.tile_size * 0.8f,
				  DARKGREEN);
		    //DrawTriangle(a, b, c, DARKGREEN);
		} else if (tile == FOREST_TREE) {
		    DrawRectangle((x * game.tile_size),
				  (y * game.tile_size),
			          game.tile_size, game.tile_size,
				  BROWN);
		    DrawRectangle((x * game.tile_size), 
                                  (y * game.tile_size),
			          game.tile_size, game.tile_size * 0.8f,
				  DARKGREEN);
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
                } else if (tile == STAIRS) {
                    DrawRectangle(x * game.tile_size, y * game.tile_size,
                                  game.tile_size, game.tile_size,
                                  BLACK);
                    DrawRectangle(x * game.tile_size, y * game.tile_size,
                                  game.tile_size, game.tile_size / 2,
                                  GRAY);
                } else if (tile == STONE_WALL) {
                    DrawRectangle(x * game.tile_size, y * game.tile_size,
                                  game.tile_size, game.tile_size,
                                  DARKGRAY);
                } else if (tile == STONE_FLOOR) {
                    DrawRectangle(x * game.tile_size, y * game.tile_size,
                                  game.tile_size, game.tile_size,
                                  GRAY);
                } else if (tile == DOOR) {
                    DrawRectangle(x * game.tile_size, y * game.tile_size,
                                  game.tile_size, game.tile_size,
                                  DARKBROWN);
                } else if (tile == LOCKED_DOOR) {
                    DrawRectangle(x * game.tile_size, y * game.tile_size,
                                  game.tile_size, game.tile_size,
                                  RED);
                } else {
		    DrawRectangle(x * game.tile_size, y * game.tile_size,
			          game.tile_size, game.tile_size, color);
		}
	    }
	}

        if (player_tile != FOREST_FLOOR) {
            for (int i = 0; i < post_render_count; i++) {
                int x = post_renders[i * 2] * game.tile_size;
                int y = post_renders[(i * 2) + 1] * game.tile_size;
                DrawRectangle(x,
                              y, game.tile_size,
                              game.tile_size, DARKGREEN);
            }
        }

        for (int i = 0; i < 20; i++) {
            Entity key = items[i];
            if (key.type == KEY) {
                DrawCircle(key.x, key.y, key.radius, key.color);
            }
        }


        if (player_tile == WATER) {
            DrawCircle(player.x,
                       player.y, player.radius * 1.5,
                       DARKBROWN);
        }
	DrawCircle(player.x,
                   player.y, player.radius,
                   player.color);

                
	EndMode2D();

        /*if (player_tile == FOREST_FLOOR) {
            BeginShaderMode(shdr_fov);
                for (int i = 0; i < post_render_count; i++) {
                    int x = post_renders[i * 2] * game.tile_size;
                    int y = post_renders[(i * 2) + 1] * game.tile_size;
                    Vector2 pos = GetWorldToScreen2D((Vector2) {x, y}, camera);
                    DrawRectangle(pos.x,
                                  pos.y, game.tile_size * camera.zoom,
                                  game.tile_size * camera.zoom, DARKGREEN);
                }
            EndShaderMode();
        }*/

        EndDrawing();
    }

    // De-Initialization
    UnloadShader(shdr_fov);
    Map_free(&map);
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
