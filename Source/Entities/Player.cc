#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include "Player.hh"

Vec2f Player::speed = {10.0f, 10.0f};
Player* Player::player = new Player;

Vec2i Player::hitbox = Vec2i{75, 75};
SDL_Rect Player::player_model = SDL_Rect{pos.v1, pos.v2, hitbox.v1, hitbox.v2};

Player* Player::GetPlayerInstace() {
    return Player::player;
}

Vec2i Player::pos = Vec2i{10, 10};
Vec2i Player::GetPlayerPos() {
    return Player::pos;
}

void Player::Move(const MoveOpts move_options) {
    switch (move_options) {
        case LEFT: {
            pos.v1 -= speed.v1;
            break;
        }
        case RIGHT: {
            pos.v1 += speed.v1;
            break;
        }
        case UP: {
            pos.v2 -= speed.v1;  // It's inverted because SDL renders from top left to bottom right
            break;
        }
        case DOWN: {
            pos.v2 += speed.v1;
            break;
        }
        default: {
            break;
        };
    }
}

void Player::DefinePlayerSpeed(float f1, float f2) {
    speed = {
        f1,
        f2,
    };
}

void Player::CalcPlayerSpeed() {
}

void Player::Draw(SDL_Renderer* renderer) {
    Player::player_model = SDL_Rect{pos.v1, pos.v2, hitbox.v1, hitbox.v2};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &player_model);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}
