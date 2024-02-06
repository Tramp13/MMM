#include "raylib.h"
#include "mmm.h"

void Player_update(Entity *self) {}

Entity Player_create(Game *game) {
    Entity entity;
    entity.x = 0;
    entity.y = 0;
    entity.radius = game->tile_size / 2;
    entity.speed = 0;
    entity.color = YELLOW;
    entity.type = 0;
    entity.update = &Player_update;
    
    return entity;
}
