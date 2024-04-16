#pragma once

#define ENTITY_MAX 1024

#define GRAVITY 200.0f
#define MAX_X_SPEED 10
#define MIN_X_SPEED -10
#define MAX_Y_SPEED 400
#define MIN_Y_SPEED -400

enum MoveOptions {
    NONE = 0,
    LEFT = 1,
    RIGHT,
    UP,
    DOWN,

};

struct Vec2i {
    int x;
    int y;
};

struct Vec2f {
    float x;
    float y;
};
