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
    game.tile_size = 32;

    InitWindow(game.screen_w, game.screen_h, "Mango Mango Monsters");

    Map map = Map_create(50, 25);

    Map_perlinify(&map, seed);

    Map_enhanceForests(&map);

    Entity player = Player_create(&game);
    /*player.x = game.tile_size / 2;
    player.y = game.tile_size / 2;*/

    //find free space for player
    Vector2 player_pos = Map_findRandomTile(&map, GRASS);
    player.x = (int) player_pos.x;
    player.y = (int) player_pos.y;
    map.tiles[player.y][player.x] = STAIRS;
    int x_speed, y_speed;
    int reg_speed;

    // Define the camera to look into our 3d world (position, target, up vector)
    printf("pass0");
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
    printf("pass1");

    printf("pass2");
    SetTargetFPS(60);
    printf("pass3");

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        // Draw
        BeginDrawing();

	ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        for (int y = 0; y < map.h; y++) {
            for (int x = 0; x < map.w; x++) {
                int tile = map.tiles[y][x];
                if (tile == GRASS || tile == FOREST_FLOOR) {
                    DrawCube((Vector3){x, 0, y - 5}, 1.0f, 1.0f, 1.0f, GREEN);
                }
                if (tile == SAND) {
                    DrawCube((Vector3){x, 0, y - 5}, 1.0f, 1.0f, 1.0f, WHITE);
                }
                if (tile == TREE || tile == DEEP_TREE || tile == FOREST_TREE) {
                    DrawCube((Vector3){x, 1, y - 5}, 1.0f, 1.0f, 1.0f, DARKGREEN);
                }
            }
        }

        /*if (cameraMode == CAMERA_THIRD_PERSON)
        {
            DrawCube(camera.target, 0.5f, 0.5f, 0.5f, PURPLE);
            DrawCubeWires(camera.target, 0.5f, 0.5f, 0.5f, DARKPURPLE);
        }*/

        EndMode3D();

        EndDrawing();
    }

    // De-Initialization
    Map_free(&map);
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
