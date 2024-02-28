#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include "Player.hh"

//------------------------------------------------------------------------------

Player* Player::player = new Player;

Player* Player::GetPlayerInstace() { return Player::player; }

Vec2f* Player::GetPlayerPos() { return &pos; }  // not used need fixing

void Player::Move(const MoveOpts move_options) {
    switch (move_options) {
        case LEFT: {
            pos.x -= velocity.x;
            break;
        }
        case RIGHT: {
            pos.x += velocity.x;
            break;
        }
        case UP: {
            pos.y -= 10;  // its subtracted because SDL renders from top left to bottom right
            break;
        }
        case DOWN: {
            pos.y += velocity.y;
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
