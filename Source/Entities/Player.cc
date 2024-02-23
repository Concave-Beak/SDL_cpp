#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include "Player.hh"

float Player::speed = 10.0f;
Player* Player::player = new Player;
Position Player::new_pos = Position{0, 0};

Player* Player::GetPlayerInstace() {
    return Player::player;
}

Position Player::GetPlayerPos() {
    return Player::new_pos;
}

void Player::Move(const MoveOpts move_options) {
    switch (move_options) {
        case LEFT: {
            new_pos.x -= speed;
            break;
        }
        case RIGHT: {
            new_pos.x += speed;
            break;
        }
        case UP: {
            new_pos.y -= speed;  // It's inverted because SDL renders from top left to bottom right
            break;
        }
        case DOWN: {
            new_pos.y += speed;
            break;
        }
        default: {
            break;
        };
    }
}

void Player::DefinePlayerSpeed(float f) {
    speed = f;
}

void Player::CalcPlayerSpeed() {
}

void Player::Draw(SDL_Renderer* renderer) {
    SDL_Rect rect = SDL_Rect{
        new_pos.x,
        new_pos.y,
        hitbox.w,
        hitbox.h,
    };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}
