#include "../../../include/game/entities/Player.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <cmath>

#include "../../../lib/utils/math_utils.hh"
#include "../../../lib/utils/sdl_utils.hh"

Player* Player::playerInstance = new Player;

// Vec2<float> Player::GetPos() { return Entity::GetPos(); }
// Vec2<float> Player::GetVelocityNow() { return Entity::GetVelocityNow(); }

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
    // Vec2<float> attackSpawnPos = {Player::GetPos().x, Player::GetPos().y};
    currentItemHolding.Attack(GetEntity(), positionNow, angleFacing);
}

void Player::Handle(const Vec2<int>& mousePos_) {
    playerInstance->mousePos = mousePos_;
}

void Player::Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer) {
    SDL_Rect playerModel = {
        // rectangles for now, because I dont have proper models
        (int)positionNow.x - cameraPos.x,
        (int)positionNow.y - cameraPos.y,
        hitbox.x,
        hitbox.y,
    };
    scc(SDL_SetRenderDrawColor(renderer, RED, 0xff)).Handle();
    scc(SDL_RenderFillRect(renderer, &playerModel)).Handle();
    DrawLineOfSight(mousePos, cameraPos, renderer);
}

void Player::SetFacingAngle(const Vec2<int>& mousePos, const Vec2<int>& cameraPos) {
    int dx = mousePos.x - (positionNow.x - cameraPos.x);
    int dy = mousePos.y - (positionNow.y - cameraPos.y);
    angleFacing = atan2(dy, dx);
}

void Player::DrawLineOfSight(const Vec2<int>& mousePos, const Vec2<int>& cameraPos, SDL_Renderer* renderer) {
    float lineLength = 80;
    SetFacingAngle(mousePos, cameraPos);

    Vec2<int> lineBegin = {
        int(positionNow.x - cameraPos.x),
        int(positionNow.y - cameraPos.y),
    };

    Vec2<int> lineEnd = GetSightLineEnd(cameraPos, lineLength);

    SDL_SetRenderDrawColor(renderer, WHITE, 0xff);
    SDL_RenderDrawLine(renderer,
                       lineBegin.x, lineBegin.y,
                       lineEnd.x, lineEnd.y);
}

Vec2<int> Player::GetSightLineEnd(const Vec2<int>& cameraPos, float lineLength) {
    return Vec2<int>{
        int((positionNow.x - cameraPos.x) + lineLength * cos(angleFacing)),
        int((positionNow.y - cameraPos.y) + lineLength * sin(angleFacing))};
}
