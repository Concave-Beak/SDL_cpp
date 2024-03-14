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
            velocity.x -= 5;
            break;
        }
        case RIGHT: {
            velocity.x += 5;
            break;
        }
        case UP: {
            pos.y -= 10;
            velocity.y -= 400;
            break;
        }
        case DOWN: {
            pos.y += 10;
            break;
        }
        default: {
            break;
        };
    }
}

void Player::CalcPlayerSpeed() {}  // TODO

void Player::Draw(SDL_Renderer* renderer) {
    Player::player_model = {(int)pos.x, (int)pos.y, hitbox.x, hitbox.y};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &player_model);
}

//------------------------------------------------------------------------------
