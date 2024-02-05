#include "mmm.h"
#include "raylib.h"

Map Map_create(int w, int h) {
    Map map;
    map.w = w;
    map.h = h;
    
    return map;
}
