#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include "Player.hh"

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
            velocity.x -= accelSpeed.x;
            break;
        }
        case RIGHT: {
            velocity.x += accelSpeed.x;
            break;
        }
        case UP: {
            // I need to -1 so it doesn't conflict with the physics engine
            pos.y -= 1;  // TODO: fix this
            velocity.y -= accelSpeed.y;
            break;
        }
        case DOWN: {
            break;
        }
        default: {
            break;
        };
    }
}

void Player::Draw(SDL_Renderer* renderer) {
    Player::player_model = {(int)pos.x, (int)pos.y, hitbox.x, hitbox.y};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &player_model);
}

//------------------------------------------------------------------------------
