#include "../../include/entities/Player.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <cmath>

#include "../../include/entities/Camera.hh"
#include "../../lib/utils/engine_utils.hh"
#include "../../lib/utils/math_utils.hh"

//------------------------------------------------------------------------------

Player* Player::player = new Player;

Player* Player::GetPlayerInstace() { return Player::player; }

Vec2i Player::GetHitboxInfo() {
    return hitbox;
}

void Player::Move(const MoveOptions moveOpt) {
    switch (moveOpt) {
        case LEFT: {
            if (!colidedLeft) {
                velocity.x -= accelSpeed.x;
            }
            facing = LEFT;
            break;
        }
        case RIGHT: {
            if (!colidedRight) {
                velocity.x += accelSpeed.x;
            }
            facing = RIGHT;
            break;
        }
        case UP: {
            if (colidedDown && !colidedUp) {
                velocity.y -= accelSpeed.y;
            }
            break;
        }
        case DOWN: {
            if (isAbovePlatform) {
                pos.y += 6;               // 6 is the safest i've found given the height
                colidedDown = false;      // of the plaform in Engine.cc. This needs to be done
                isAbovePlatform = false;  // to place the player bellow the platform's top
            }  // TODO: change how this works
            break;
        }
        default: {
            break;
        };
    }
}

void Player::PrepareToDash(MoveOptions moveOpt, float startTick, SDL_Renderer* renderer, float* timeMultiplier) {
    float modOfAngleDash = fmod(angleDash, 360);
    angleDash = modOfAngleDash;
    // decrease timeMultiplier
    if (*timeMultiplier > 0.1) {
        *timeMultiplier += (0.1 - *timeMultiplier) / 25;
    }
    switch (moveOpt) {
        // each angle position represents an angle
        case NONE: {
            break;
        }
        case LEFT: {
            angleDash += (180 - angleDash) / 20.0;
            break;
        }
        case RIGHT: {
            float addDash = (0 - angleDash) / 20.0f;
            if (angleDash > 180) {
                addDash = (360 - angleDash) / 20.0f;
            }
            angleDash += addDash;
            break;
        }
        case UP: {  // SDL flips "up" and "down"
            float addDash = (270 - angleDash) / 20.0f;
            if (angleDash > -90 && angleDash < 90) {
                addDash = (-90 - angleDash) / 20.0f;
            }
            angleDash += addDash;
            break;
        }
        case DOWN: {
            float addDash = (90 - angleDash) / 20.0f;
            if (angleDash > 270) {
                addDash = (450 - angleDash) / 20.0f;
            }
            angleDash += addDash;
            break;
        }
    }
    float angleDashInRadians = DegreesToRadians(angleDash);

    int lineDistance = 100;
    Vec2i pointStart = {(int)pos.x + hitbox.x / 2,
                        (int)pos.y + hitbox.y / 2};
    Vec2i pointEnd = {int(pointStart.x + lineDistance * cos(angleDashInRadians)),
                      int(pointStart.y + lineDistance * sin(angleDashInRadians))};

    Vec2f cameraPos = Camera::pos;
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0x00);
    SDL_RenderDrawLine(renderer, pointStart.x - cameraPos.x, pointStart.y - cameraPos.y, pointEnd.x - cameraPos.x, pointEnd.y - cameraPos.y);
    (void)startTick;
}

void Player::Dash() {
    Vec2f dashStrenght = {300.0f, 300.0f};
    float dashAngleInRadians = DegreesToRadians(angleDash);
    Vec2f dashVel = {dashStrenght.x * cos(dashAngleInRadians), dashStrenght.y * sin(dashAngleInRadians)};
    velocity.x = dashVel.x;
    velocity.y = dashVel.y;
    isPreparingToDash = false;
}

//------------------------------------------------------------------------------
