#include "raylib.h"
#include "mmm.h"
#include "perlin.h"

// Program main entry point
int main(void)
{
    // Initialization
    Game game;
    game.screen_w = 800;
    game.screen_h = 450;
    game.scale = 1;
    game.tile_size = 16;

    Map map = Map_create(400, 400);

    InitWindow(game.screen_w, game.screen_h, "Mango Mango Monsters");

    SetTargetFPS(60);            // Set our game to run at 60 frames-per-second
    
    Entity player = Player_create(&game);
    player.x = 100;
    player.y = 100;

    Camera2D camera = { 0 };
    camera.target = (Vector2){player.x, player.y};
    camera.offset = (Vector2){game.screen_w / 2, game.screen_h / 2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    int tiles[map.w * map.h];
    for (int y = 0; y < map.h; y++) {
	for (int x = 0; x < map.w; x++) {
	    float tile = perlin2d(x, y, 0.025, 4);
	    int tile_index = (y * map.w) + x;
	    if (tile < 0.5) {
		tiles[tile_index] = 0;
	    } else {
		tiles[tile_index] = 1;
	    }
	}
    }

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        // TODO: Update your variables here
	if (IsKeyDown(KEY_LEFT)) player.x -= 2;
	if (IsKeyDown(KEY_RIGHT)) player.x += 2;
	if (IsKeyDown(KEY_UP)) player.y -= 2;
	if (IsKeyDown(KEY_DOWN)) player.y += 2;

	camera.target = (Vector2){player.x, player.y};

        // Draw
        BeginDrawing();

	ClearBackground(RAYWHITE);

	BeginMode2D(camera);
	
	int half_screen_tiles_width = (game.screen_w / 2) / game.tile_size;
	int player_tile_x = player.x / game.tile_size;

	int half_screen_tiles_height = (game.screen_h / 2) / game.tile_size;
	int player_tile_y = player.y / game.tile_size;

	int top_draw_tiles = player_tile_y - half_screen_tiles_height;
	int bottom_draw_tiles = player_tile_y + half_screen_tiles_height;

	int left_draw_tiles = player_tile_x - half_screen_tiles_width;
	int right_draw_tiles = player_tile_x + half_screen_tiles_width;

	for (int y = top_draw_tiles; y < bottom_draw_tiles; y++) {
	   for (int x = left_draw_tiles; x < right_draw_tiles; x++) {
		Color color;
		int tile_index = (y * map.w) + x;
		if (tiles[tile_index] == 0) {
		    color = BLUE;
		} else {
		    color = GREEN;
		}
		DrawRectangle(x * game.tile_size, y * game.tile_size,
			      game.tile_size, game.tile_size, color);
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
