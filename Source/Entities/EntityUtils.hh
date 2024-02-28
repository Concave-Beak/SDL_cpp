#ifndef __ENTITY_HEADER__
#define __ENTITY_HEADER__

#define ENTITY_MAX 1024

struct Vec2i {
    int x;
    int y;
    Vec2i(){};  // TODO: test if this line and the bellow is necessary
    Vec2i(int _x, int _y) : x(_x), y(_y){};
};

struct Vec2f {
    int x;
    int y;
    Vec2f(){};  // TODO: same here
    Vec2f(float _x, float _y) : x(_x), y(_y){};
};

#endif
