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

    Texture2D urizen = LoadTexture("urizen.png");
    Rectangle grass_rect = {40, 118, 12, 12};
    Rectangle tree_rect = {1, 105, 12, 12};
    Rectangle water_rect = {118, 534, 12, 12};
    Rectangle sand_rect = {105, 443, 12, 12};
    Rectangle shade_rect = {144, 443, 12, 12};
    Rectangle wall_rect = {118, 534, 12, 12};
    Rectangle floor_rect = {105, 443, 12, 12};
    Rectangle stair_rect = {105, 443, 12, 12};

    SetTargetFPS(60);            // Set our game to run at 60 frames-per-second
    
    Map map = Map_create(400, 400);

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
            map = Map_createLab();
            player_pos = Map_findRandomTile(&map, STONE_FLOOR);
            player.x = player_pos.x * game.tile_size;
            player.y = player_pos.y * game.tile_size;
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
        Rectangle dest_rect;
        Rectangle src_rect;
        Vector2 origin = (Vector2) {0, 0};

	for (int y = top; y <= bottom; y++) {
	   for (int x = left; x <= right; x++) {
	        dest_rect = (Rectangle) {(x * game.tile_size),
			     (y * game.tile_size),
			     game.tile_size, game.tile_size};
		int tile = getTile(&map, x, y);
		if (tile == WATER) {
                    DrawTexturePro(urizen, water_rect, dest_rect, origin, 0,
                                   WHITE);
		} else if (tile == SAND) {
                    DrawTexturePro(urizen, sand_rect, dest_rect, origin, 0,
                                   WHITE);
		} else if (tile == GRASS) {
                    DrawTexturePro(urizen, grass_rect, dest_rect, origin, 0,
                                   WHITE);
		} else if (tile == TREE) {
                    DrawTexturePro(urizen, tree_rect, dest_rect, origin, 0,
                                   WHITE);
		} else if (tile == FOREST_TREE) {
                    DrawTexturePro(urizen, tree_rect, dest_rect, origin, 0,
                                   WHITE);
                    post_renders[(post_render_count * 2)] = x;
                    post_renders[(post_render_count * 2) + 1] = y;
                    post_render_count++;
                } else if (tile == FOREST_FLOOR) {
                    DrawTexturePro(urizen, grass_rect, dest_rect, origin, 0,
                                   WHITE);
                    post_renders[(post_render_count * 2)] = x;
                    post_renders[(post_render_count * 2) + 1] = y;
                    post_render_count++;
                } else if (tile == DEEP_TREE) {
                    DrawTexturePro(urizen, tree_rect, dest_rect, origin, 0,
                                   WHITE);
                    post_renders[(post_render_count * 2)] = x;
                    post_renders[(post_render_count * 2) + 1] = y;
                    post_render_count++;
                } else if (tile == STAIRS) {
                    DrawTexturePro(urizen, stair_rect, dest_rect, origin, 0,
                                   WHITE);
                } else if (tile == STONE_WALL) {
                    DrawTexturePro(urizen, wall_rect, dest_rect, origin, 0,
                                   WHITE);
                } else if (tile == STONE_FLOOR) {
                    DrawTexturePro(urizen, floor_rect, dest_rect, origin, 0,
                                   WHITE);
                } else if (tile == DOOR) {
                    DrawTexturePro(urizen, floor_rect, dest_rect, origin, 0,
                                   WHITE);
                }
	    }
	}

        for (int i = 0; i < post_render_count; i++) {
            int x = post_renders[i * 2] * game.tile_size;
            int y = post_renders[(i * 2) + 1] * game.tile_size;
            dest_rect = (Rectangle) {(x * game.tile_size),
                         (y * game.tile_size),
                         game.tile_size, game.tile_size};
            DrawTexturePro(urizen, shade_rect, dest_rect, origin, 0,
                           WHITE);
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

        BeginShaderMode(shdr_fov);
            for (int i = 0; i < post_render_count; i++) {
                int x = post_renders[i * 2] * game.tile_size;
                int y = post_renders[(i * 2) + 1] * game.tile_size;
                Vector2 pos = GetWorldToScreen2D((Vector2) {x, y}, camera);
                dest_rect = (Rectangle) {pos.x, pos.y,
                         game.tile_size * camera.zoom,
                         game.tile_size * camera.zoom};
                DrawTexturePro(urizen, shade_rect, dest_rect, origin, 0,
                               WHITE);
            }
        EndShaderMode();

        EndDrawing();
    }

    // De-Initialization
    UnloadShader(shdr_fov);
    Map_free(&map);
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
