#include "../../../include/game/entities/Player.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <cmath>
#include <ctime>

#include "../../../include/game/items/ItemFactory.hh"
#include "../../../include/main/Level.hh"
#include "../../../lib/utils/math_utils.hh"
#include "../../../lib/utils/sdl_utils.hh"

Vec2<int> GetSightLineEnd(Vec2<int> positionNow, SDL_Rect model, float angleFacing, Vec2<int> cameraPos, float lineLength);
float SetFacingAngle(Vec2<float> positionNow, Vec2<int> mousePos, Vec2<int> cameraPos);

std::shared_ptr<Player> Player::Create() {
    std::shared_ptr<Player> player(new Player());
    return player;
}

std::shared_ptr<Player> Player::instance = Create();

std::shared_ptr<Player> Player::Instance() {
    instance->InitInventory();
    instance->entityAttributes.model = {0, 0, 64, 64};
    instance->entityAttributes.walkingSpeed = {10, 75};  // TODO make a init player function
    instance->entityAttributes.runningSpeed = {20, 125};
    return instance;
}

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

//------------------------------------------------------------------------------

std::shared_ptr<PlayerHandler> PlayerHandler::Init(SDL_Renderer* renderer_) {
    handler.renderer = renderer_;
    return std::make_shared<PlayerHandler>(handler);
}

PlayerHandler& PlayerHandler::Instance() { return handler; }
PlayerHandler PlayerHandler::handler = PlayerHandler();

Vec2<float> PlayerHandler::GetRunningSpeed() { return playerInstance->entityAttributes.runningSpeed; }
Vec2<float> PlayerHandler::GetWalkingSpeed() { return playerInstance->entityAttributes.walkingSpeed; }
Vec2<float> PlayerHandler::GetPos() { return playerInstance->entityAttributes.positionNow; }
SDL_Rect PlayerHandler::GetModel() { return playerInstance->entityAttributes.model; }

void PlayerHandler::Handle(Vec2<int> mousePos_, Vec2<int> cameraPos, float timeDelta, float timeMultiplier, bool isPaused) {
    playerInstance->angleFacing = SetFacingAngle(playerInstance->entityAttributes.positionNow, mousePos_, cameraPos);
    HandleVelocity(timeDelta, timeMultiplier, isPaused);
    HandleCollisions(timeDelta, timeMultiplier, isPaused);
    DrawSightLine(cameraPos, renderer);
    Draw(cameraPos);
}

bool PlayerHandler::GetCollidedInformation(Direction direction) {
    switch (direction) {
        case Direction::UP: {
            return playerInstance->collisionAttributes.collidedUp;
        }
        case Direction::LEFT: {
            return playerInstance->collisionAttributes.collidedUp;
        }
        case Direction::RIGHT: {
            return playerInstance->collisionAttributes.collidedUp;
        }
        case Direction::DOWN: {
            return playerInstance->collisionAttributes.collidedUp;
        }
    }
    return false;
}

void PlayerHandler::ChargeAttack() {
    if (playerInstance->currentItemHolding != nullptr) {
        playerInstance->currentItemHolding->ChargeAttack();
    }
}

void PlayerHandler::ReleaseAttack() {
    if (playerInstance->currentItemHolding != nullptr) {
        playerInstance->currentItemHolding->ReleaseAttack(&playerInstance->entityAttributes, &playerInstance->combatAttributes, playerInstance->angleFacing);
    }
}

void PlayerHandler::MovePlayer(Direction direction, bool isPaused, bool isRunning) {
    if (isPaused) return;
    if (GetCollidedInformation(direction)) return;

    Vec2<float> accelSpeed = playerInstance->entityAttributes.walkingSpeed;
    if (isRunning) accelSpeed = playerInstance->entityAttributes.runningSpeed;
    switch (direction) {
        case Direction::LEFT: {
            playerInstance->entityAttributes.velocityNow.x -= accelSpeed.x;
            playerInstance->entityAttributes.facing = Direction::LEFT;
            break;
        }
        case Direction::RIGHT: {
            playerInstance->entityAttributes.velocityNow.x += accelSpeed.x;
            playerInstance->entityAttributes.facing = Direction::RIGHT;
            break;
        }
        case Direction::UP: {
            if (!playerInstance->collisionAttributes.collidedDown) break;
            playerInstance->entityAttributes.velocityNow.y -= accelSpeed.y;
            playerInstance->collisionAttributes.collidedDown = false;
            break;
        }
        case Direction::DOWN: {
            if (!playerInstance->collisionAttributes.isAbovePlatform) return;
            playerInstance->entityAttributes.positionNow.y += 6;  // TODO: change how this works
            playerInstance->collisionAttributes.collidedDown = false;
            playerInstance->collisionAttributes.isAbovePlatform = false;
            break;
        }
    }
}

void PlayerHandler::HandleVelocity(float timeDelta, float timeMultiplier, bool isPaused) {
    if (isPaused) return;

    if (playerInstance->entityAttributes.velocityNow.y > MAX_Y_SPEED) {
        playerInstance->entityAttributes.velocityNow.y = MAX_Y_SPEED;
    }
    if (playerInstance->entityAttributes.velocityNow.y < -MAX_Y_SPEED) {
        playerInstance->entityAttributes.velocityNow.y = -MAX_Y_SPEED;
    }

    if (playerInstance->entityAttributes.velocityNow.x > MAX_X_SPEED) {
        playerInstance->entityAttributes.velocityNow.x = MAX_X_SPEED;
    }
    if (playerInstance->entityAttributes.velocityNow.x < -MAX_X_SPEED) {
        playerInstance->entityAttributes.velocityNow.x = -MAX_X_SPEED;
    }

    if (!playerInstance->collisionAttributes.collidedDown) {
        playerInstance->entityAttributes.velocityNow.y += timeDelta * GRAVITY * timeMultiplier;
        playerInstance->entityAttributes.positionNow.y += playerInstance->entityAttributes.velocityNow.y * timeDelta * timeMultiplier;
    }

    if (!playerInstance->collisionAttributes.collidedDown) playerInstance->collisionAttributes.surfaceAttrition = AIR_ATTRITION;

    playerInstance->entityAttributes.velocityNow.x -= timeDelta * playerInstance->collisionAttributes.surfaceAttrition * timeMultiplier * playerInstance->entityAttributes.velocityNow.x;
    playerInstance->entityAttributes.positionNow.x += playerInstance->entityAttributes.velocityNow.x * timeMultiplier * timeDelta;
}

void PlayerHandler::ResetCollisionState() {
    playerInstance->collisionAttributes.collidedDown = false, playerInstance->collisionAttributes.collidedUp = false,
    playerInstance->collisionAttributes.collidedLeft = false, playerInstance->collisionAttributes.collidedRight = false;
    playerInstance->collisionAttributes.isAbovePlatform = false;
}

void PlayerHandler::HandleVerticalCollision(SDL_Rect entityRect, const LevelItem& levelItem,
                                            float timeDelta, float timeMultiplier) {
    float levelItemTop = float(levelItem.pos.y),
          levelItemBottom = float(levelItem.pos.y + levelItem.rect.h);

    bool hitFeet = IsSideColliding(entityRect, levelItem.rect, Direction::DOWN, playerInstance->entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitFeet) {
        playerInstance->collisionAttributes.collidedDown = true;
        playerInstance->collisionAttributes.surfaceAttrition = levelItem.attritionCoefficient;
        playerInstance->entityAttributes.positionNow.y = levelItemTop - float(playerInstance->entityAttributes.model.h);
        playerInstance->entityAttributes.velocityNow.y = 0;
        if (levelItem.collisionType == PLATFORM) playerInstance->collisionAttributes.isAbovePlatform = true;
    }

    if (levelItem.collisionType == CollisionType::PLATFORM) return;

    bool hitHead = IsSideColliding(entityRect, levelItem.rect, Direction::UP, playerInstance->entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitHead) {
        playerInstance->collisionAttributes.collidedUp = true;
        playerInstance->entityAttributes.positionNow.y = levelItemBottom;
        playerInstance->entityAttributes.velocityNow.y = -playerInstance->entityAttributes.velocityNow.y * SURFACE_BOUNCE;
    }
}

void PlayerHandler::HandleHorizontalCollision(SDL_Rect model, const LevelItem& levelItem,
                                              float timeDelta, float timeMultiplier) {
    if (levelItem.collisionType == PLATFORM) return;

    float levelItemLeft = float(levelItem.pos.x),
          levelItemRight = float(levelItem.pos.x + levelItem.rect.w);

    bool hitRight = IsSideColliding(model, levelItem.rect, Direction::RIGHT, playerInstance->entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitRight) {
        playerInstance->collisionAttributes.collidedRight = true;
        playerInstance->entityAttributes.velocityNow.x = -playerInstance->entityAttributes.velocityNow.x * SURFACE_BOUNCE;
        playerInstance->entityAttributes.positionNow.x = levelItemLeft - float(playerInstance->entityAttributes.model.w);
    }

    bool hitLeft = IsSideColliding(model, levelItem.rect, Direction::LEFT, playerInstance->entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitLeft) {
        playerInstance->collisionAttributes.collidedLeft = true;
        playerInstance->entityAttributes.velocityNow.x = -playerInstance->entityAttributes.velocityNow.x * SURFACE_BOUNCE;
        playerInstance->entityAttributes.positionNow.x = levelItemRight;
    }
}

void PlayerHandler::UpdateModel() {
    playerInstance->entityAttributes.model = SDL_Rect{
        .x = int(playerInstance->entityAttributes.positionNow.x),
        .y = int(playerInstance->entityAttributes.positionNow.y),
        .w = playerInstance->entityAttributes.model.w,
        .h = playerInstance->entityAttributes.model.h,
    };
}

void PlayerHandler::HandleCollisions(float timeDelta, float timeMultiplier, bool isPaused) {
    if (isPaused) return;

    ResetCollisionState();

    for (LevelItem levelItem : Level::collisions) {
        UpdateModel();
        HandleVerticalCollision(playerInstance->entityAttributes.model, levelItem, timeDelta, timeMultiplier);  // TODO remove model
        UpdateModel();
        HandleHorizontalCollision(playerInstance->entityAttributes.model, levelItem, timeDelta, timeMultiplier);
    }
}

void PlayerHandler::Draw(Vec2<int> cameraPos) {
    Vec2<float> positionNow = playerInstance->entityAttributes.positionNow;
    SDL_Rect playerModel = {
        (int)positionNow.x - cameraPos.x,
        (int)positionNow.y - cameraPos.y,
        playerInstance->entityAttributes.model.w,
        playerInstance->entityAttributes.model.h,
    };
    scc(SDL_SetRenderDrawColor(renderer, RED, 0xff)).Handle();
    scc(SDL_RenderFillRect(renderer, &playerModel)).Handle();
}

void PlayerHandler::DrawSightLine(Vec2<int> cameraPos, SDL_Renderer* renderer) {
    float lineLength = 80;

    Vec2<int> lineBegin = {
        int(playerInstance->entityAttributes.positionNow.x - cameraPos.x + playerInstance->entityAttributes.model.w * 0.5f),
        int(playerInstance->entityAttributes.positionNow.y - cameraPos.y + playerInstance->entityAttributes.model.h * 0.5f),
    };

    Vec2<int> lineEnd = GetSightLineEnd(playerInstance->entityAttributes.positionNow, playerInstance->entityAttributes.model, playerInstance->angleFacing, cameraPos, lineLength);

    scc(SDL_SetRenderDrawColor(renderer, WHITE, 0xff)).Handle();
    scc(SDL_RenderDrawLine(renderer,
                           lineBegin.x, lineBegin.y,
                           lineEnd.x, lineEnd.y))
        .Handle();
}

Vec2<int> GetSightLineEnd(Vec2<int> positionNow, SDL_Rect model, float angleFacing, Vec2<int> cameraPos, float lineLength) {
    return Vec2<int>{
        int((positionNow.x - cameraPos.x) + model.w * 0.5f + lineLength * cos(angleFacing)),
        int((positionNow.y - cameraPos.y) + model.h * 0.5f + lineLength * sin(angleFacing))};
}

float SetFacingAngle(Vec2<float> positionNow, Vec2<int> mousePos, Vec2<int> cameraPos) {
    int dx = mousePos.x - (positionNow.x - cameraPos.x);
    int dy = mousePos.y - (positionNow.y - cameraPos.y);
    return atan2(dy, dx);
}
