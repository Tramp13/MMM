#include "raylib.h"
#include "mmm.h"

void Entity_update(Entity *self) {}

Entity Entity_create(Game *game) {
    Entity entity;
    entity.x = 0;
    entity.y = 0;
    entity.radius = game->tile_size / 2;
    entity.speed = 0;
    entity.color = BLACK;
    entity.type = 0;
    entity.update = &Entity_update;
    
    return entity;
}
