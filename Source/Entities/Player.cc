#include "Player.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include "../../Include/Headers/Utils.hh"

//------------------------------------------------------------------------------

Player* Player::player = new Player;

Player* Player::GetPlayerInstace() { return Player::player; }

Vec2f* Player::GetPlayerPos() { return &pos; }  // not used need fixing

Vec2i Player::GetHitboxInfo() {
    return hitbox;
}

void Player::Move(const MoveOptions move_options) {
    switch (move_options) {
        case LEFT: {
            if (!colidedLeft) {
                velocity.x -= accelSpeed.x;
            }
            break;
        }
        case RIGHT: {
            if (!colidedRight) {
                velocity.x += accelSpeed.x;
            }
            break;
        }
        case UP: {
            if (colidedDown && !colidedUp) {
                velocity.y -= accelSpeed.y;
                break;
            }
        }
        case DOWN: {
            if (isAbovePlatform) {
                velocity.y += 10;
                pos.y += 6;               // 6 is the safest i've found given the height
                colidedDown = false;      // of the plaform in Engine.cc. This needs to be done
                isAbovePlatform = false;  // to place the player bellow the platform's top
            }                             // TODO: change how this works
            break;
        }
        default: {
            break;
        };
    }
}

//------------------------------------------------------------------------------
