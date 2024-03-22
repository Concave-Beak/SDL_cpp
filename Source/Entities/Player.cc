#include "Player.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include <cstdio>
#include <vector>

//------------------------------------------------------------------------------

Player* Player::player = new Player;

Player* Player::GetPlayerInstace() { return Player::player; }

Vec2f* Player::GetPlayerPos() { return &pos; }  // not used need fixing

Vec2i Player::GetHitboxInfo() {
    return hitbox;
}

void Player::Move(const MoveOpts move_options) {
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
                pos.y += 3.5;             // 3.5 is the safets i've found given the height
                colidedDown = false;      // of the plaform in Engine.cc. This needs to be done
                isAbovePlatform = false;  // to place the player bellow the platform's top
            }
            break;
        }
        default: {
            break;
        };
    }
}

void Player::Draw(SDL_Renderer* renderer) {
    std::vector<int> a;

    a.resize(100);
    Player::player_model = {(int)pos.x, (int)pos.y, hitbox.x, hitbox.y};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &player_model);
}

//------------------------------------------------------------------------------
