#ifndef __ENTITY_HEADER__
#define __ENTITY_HEADER__

#define ENTITY_MAX 1024

struct Vec2i {
    int v1;
    int v2;
    Vec2i(){};
    Vec2i(int _x, int _y) : v1(_x), v2(_y){};
};

struct Vec2f {
    float v1;
    float v2;
    Vec2f(){};
    Vec2f(float _x, float _y) : v1(_x), v2(_y){};
};

enum EntityType {
    ENEMY = 0,
    PLAYER = 1,
};

#endif
