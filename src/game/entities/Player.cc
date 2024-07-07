#include "../../../include/game/entities/Player.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <cmath>

#include "../../../include/game/items/ItemFactory.hh"
#include "../../../lib/utils/math_utils.hh"
#include "../../../lib/utils/sdl_utils.hh"

Player::Player() {
    model = {0, 0, 64, 64};
}
Player* Player::instance = new Player();

Player* Player::Instance() {
    instance->InitInventory();
    return Player::instance;
}
Vec2<float> Player::GetRunningSpeed() { return runningSpeed; }
Vec2<float> Player::GetWalkingSpeed() { return walkingSpeed; }

void Player::InitInventory() {
    inventory.Set(0, 0, Items::ItemFactory::Instance().CreateItem(Items::ItemID::SHORTSWORD));
    currentItemHolding = inventory.At(0, 0);
}

void Player::SwitchWeapon(WeaponHand weaponHand) {
    currentItemHolding = inventory.At(0, 0);
    if (weaponHand == RIGHT_HAND) {
        currentItemHolding = inventory.At(0, 1);
    }
}

void Player::ChargeAttack() {
    if (currentItemHolding != nullptr) {
        currentItemHolding->ChargeAttack();
    }
}

void Player::ReleaseAttack() {
    if (currentItemHolding != nullptr) {
        currentItemHolding->ReleaseAttack(GetEntity(), angleFacing, &itemCooldown);
    }
}

void Player::Handle(const Vec2<int>& mousePos_) {
    instance->mousePos = mousePos_;
}

void Player::Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer) {
    SDL_Rect playerModel = {
        // rectangles for now, because I dont have proper models
        (int)positionNow.x - cameraPos.x,
        (int)positionNow.y - cameraPos.y,
        model.w,
        model.h,
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
        int(positionNow.x - cameraPos.x + model.w * 0.5f),
        int(positionNow.y - cameraPos.y + model.h * 0.5f),
    };

    Vec2<int> lineEnd = GetSightLineEnd(cameraPos, lineLength);

    SDL_SetRenderDrawColor(renderer, WHITE, 0xff);
    SDL_RenderDrawLine(renderer,
                       lineBegin.x, lineBegin.y,
                       lineEnd.x, lineEnd.y);
}

Vec2<int> Player::GetSightLineEnd(const Vec2<int>& cameraPos, float lineLength) {
    return Vec2<int>{
        int((positionNow.x - cameraPos.x) + model.w * 0.5f + lineLength * cos(angleFacing)),
        int((positionNow.y - cameraPos.y) + model.h * 0.5f + lineLength * sin(angleFacing))};
}
