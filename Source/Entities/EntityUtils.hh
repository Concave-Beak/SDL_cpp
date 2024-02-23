#ifndef __ENTITY_HEADER__
#define __ENTITY_HEADER__

#define ENTITY_MAX 1024

struct Position {
    int x;
    int y;
    Position(){};
    Position(int _x, int _y) : x(_x), y(_y){};
};
struct Hitbox {
    int w;
    int h;
    Hitbox(){};
    Hitbox(int _w, int _h) : w(_w), h(_h){};
};

enum EntityType {
    ENEMY = 0,
    PLAYER = 1,
};

#endif
