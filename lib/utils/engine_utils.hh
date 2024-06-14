#pragma once

#define ENTITY_MAX 1024

#define GRAVITY 120.0f

#define MAX_X_SPEED 400
#define MAX_Y_SPEED 400

#define MAX_PROJECTILE_SPEED_X 400
#define MAX_PROJECTILE_SPEED_Y 400

#define AIR_ATTRITION 1.5f
#define SURFACE_BOUNCE 0.2f

enum Direction {
    LEFT = 0,
    RIGHT,
    UP,
    DOWN,

};

enum FullscreenMode {
    FULLSCREEN_DEFAULT = 0,
    FULLSCREEN_WINDOWED,
    FULLSCREEN_DESKTOP,
};
