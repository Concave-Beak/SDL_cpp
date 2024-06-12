#include "../../../include/game/entities/Player.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <cmath>

#include "../../../lib/utils/math_utils.hh"
#include "../../../lib/utils/sdl_utils.hh"

Player* Player::playerInstance = new Player;

Vec2<float> Player::GetPos() { return Entity::GetPos(); }
Vec2<float> Player::GetVelocityNow() { return Entity::GetVelocityNow(); }

Player* Player::GetPlayerInstace() { return Player::playerInstance; }
Vec2<float> Player::GetRunningSpeed() { return runningSpeed; }
Vec2<float> Player::GetWalkingSpeed() { return walkingSpeed; }

Vec2<int> Player::GetHitbox() { return Entity::GetHitbox(); }

void Player::SwitchWeapon(WeaponHand weaponHand) {
    currentItemHolding = inventory[0][0];
    if (weaponHand == RIGHT_HAND) {
        currentItemHolding = inventory[0][1];
    }
}

void Player::Attack() {
    Vec2<float> attackSpawnPos = {Player::GetPos().x + Player::GetHitbox().x, Player::GetPos().y + Player::GetHitbox().y / 2.0f};
    if (Entity::GetFacingDirection() == LEFT) {
        attackSpawnPos = {Player::GetPos().x, Player::GetPos().y + Player::GetHitbox().y / 2.0f};
    }
    currentItemHolding.Attack(this->GetEntity(), attackSpawnPos);
}

void Player::Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer) {
    SDL_Rect playerModel = {
        // rectangles for now, because I dont have proper models
        (int)this->positionNow.x - cameraPos.x,
        (int)this->positionNow.y - cameraPos.y,
        this->hitbox.x,
        this->hitbox.y,
    };
    scc(SDL_SetRenderDrawColor(renderer, RED, 0xff));
    scc(SDL_RenderFillRect(renderer, &playerModel));
}
