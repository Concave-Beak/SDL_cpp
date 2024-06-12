#pragma once

#define ENTITY_MAX 1024

#define GRAVITY 200.0f

#define MAX_X_SPEED 10
#define MAX_Y_SPEED 400

#define MAX_PROJECTILE_SPEED_X 80
#define MAX_PROJECTYLE_SPEED_Y 400

#define AIR_ATTRITION 0.8f
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
