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

enum FullscreenMode {
    FULLSCREEN_DEFAULT = 0,
    FULLSCREEN_WINDOWED,
    FULLSCREEN_DESKTOP,
};
