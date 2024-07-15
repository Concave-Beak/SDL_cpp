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
    instance->creatureAttributes.model = {0, 0, 64, 64};
    instance->creatureAttributes.walkingSpeed = {10, 75};  // TODO make a init player function
    instance->creatureAttributes.runningSpeed = {20, 125};
    return instance;
}

void Player::InitInventory() {
    inventory.Set(0, 0, Items::ItemFactory::Instance().CreateItem(Items::ItemID::SHORTSWORD));
    currentItemHolding = inventory.At(0, 0);
}

void Player::SwitchWeapon(WeaponHand weaponHand) {  // Should be handled throught the PlayerHandler
    currentItemHolding = inventory.At(0, 0);
    if (weaponHand == RIGHT_HAND) {
        currentItemHolding = inventory.At(0, 1);
    }
}

Vec2<float> Player::GetRunningSpeed() { return instance->creatureAttributes.runningSpeed; }
Vec2<float> Player::GetWalkingSpeed() { return instance->creatureAttributes.walkingSpeed; }
Vec2<float> Player::GetPos() { return instance->creatureAttributes.positionNow; }
SDL_Rect Player::GetModel() { return instance->creatureAttributes.model; }
CreatureAttributes Player::GetAttribute() { return instance->creatureAttributes; }
const CreatureAttributes* Player::GetAttributeReference() { return &instance->creatureAttributes; }

//------------------------------------------------------------------------------

std::shared_ptr<PlayerHandler> PlayerHandler::Init(SDL_Renderer* renderer_) {
    handler.playerInstance = Player::Instance();
    handler.renderer = renderer_;
    return std::make_shared<PlayerHandler>(handler);
}

PlayerHandler& PlayerHandler::Instance() { return handler; }
PlayerHandler PlayerHandler::handler = PlayerHandler();

void PlayerHandler::Handle(Vec2<int> mousePos_, Vec2<int> cameraPos, float timeDelta, float timeMultiplier, bool isPaused) {
    playerInstance->angleFacing = SetFacingAngle(playerInstance->creatureAttributes.positionNow, mousePos_, cameraPos);
    HandleVelocity(timeDelta, timeMultiplier, isPaused);
    HandleCollisions(timeDelta, timeMultiplier, isPaused);
    DrawSightLine(cameraPos, renderer);
    Draw(cameraPos);
}

bool PlayerHandler::GetCollidedInformation(Direction direction) {
    switch (direction) {
        case Direction::UP: {
            return playerInstance->creatureAttributes.collisionAttributes.collidedUp;
        }
        case Direction::LEFT: {
            return playerInstance->creatureAttributes.collisionAttributes.collidedLeft;
        }
        case Direction::RIGHT: {
            return playerInstance->creatureAttributes.collisionAttributes.collidedRight;
        }
        case Direction::DOWN: {
            return playerInstance->creatureAttributes.collisionAttributes.collidedDown;
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
        playerInstance->currentItemHolding->ReleaseAttack(&playerInstance->creatureAttributes, &playerInstance->creatureAttributes.combatAttributes, playerInstance->angleFacing);
    }
}

void PlayerHandler::MovePlayer(Direction direction, bool isPaused, bool isRunning) {
    if (isPaused) return;
    if (GetCollidedInformation(direction)) return;

    Vec2<float> accelSpeed = playerInstance->creatureAttributes.walkingSpeed;
    if (isRunning) accelSpeed = playerInstance->creatureAttributes.runningSpeed;
    switch (direction) {
        case Direction::LEFT: {
            playerInstance->creatureAttributes.velocityNow.x -= accelSpeed.x;
            playerInstance->creatureAttributes.facing = Direction::LEFT;
            break;
        }
        case Direction::RIGHT: {
            playerInstance->creatureAttributes.velocityNow.x += accelSpeed.x;
            playerInstance->creatureAttributes.facing = Direction::RIGHT;
            break;
        }
        case Direction::UP: {
            if (!playerInstance->creatureAttributes.collisionAttributes.collidedDown) break;
            playerInstance->creatureAttributes.velocityNow.y -= accelSpeed.y;
            playerInstance->creatureAttributes.collisionAttributes.collidedDown = false;
            break;
        }
        case Direction::DOWN: {
            if (!playerInstance->creatureAttributes.collisionAttributes.isAbovePlatform) return;
            playerInstance->creatureAttributes.positionNow.y += 6;  // TODO: change how this works
            playerInstance->creatureAttributes.collisionAttributes.collidedDown = false;
            playerInstance->creatureAttributes.collisionAttributes.isAbovePlatform = false;
            break;
        }
    }
}

void PlayerHandler::HandleVelocity(float timeDelta, float timeMultiplier, bool isPaused) {
    if (isPaused) return;

    if (playerInstance->creatureAttributes.velocityNow.y > MAX_Y_SPEED) {
        playerInstance->creatureAttributes.velocityNow.y = MAX_Y_SPEED;
    }
    if (playerInstance->creatureAttributes.velocityNow.y < -MAX_Y_SPEED) {
        playerInstance->creatureAttributes.velocityNow.y = -MAX_Y_SPEED;
    }

    if (playerInstance->creatureAttributes.velocityNow.x > MAX_X_SPEED) {
        playerInstance->creatureAttributes.velocityNow.x = MAX_X_SPEED;
    }
    if (playerInstance->creatureAttributes.velocityNow.x < -MAX_X_SPEED) {
        playerInstance->creatureAttributes.velocityNow.x = -MAX_X_SPEED;
    }

    if (!playerInstance->creatureAttributes.collisionAttributes.collidedDown) {
        playerInstance->creatureAttributes.velocityNow.y += timeDelta * GRAVITY * timeMultiplier;
        playerInstance->creatureAttributes.positionNow.y += playerInstance->creatureAttributes.velocityNow.y * timeDelta * timeMultiplier;
    }

    if (!playerInstance->creatureAttributes.collisionAttributes.collidedDown) playerInstance->creatureAttributes.collisionAttributes.surfaceAttrition = AIR_ATTRITION;

    playerInstance->creatureAttributes.velocityNow.x -= timeDelta * playerInstance->creatureAttributes.collisionAttributes.surfaceAttrition * timeMultiplier * playerInstance->creatureAttributes.velocityNow.x;
    playerInstance->creatureAttributes.positionNow.x += playerInstance->creatureAttributes.velocityNow.x * timeMultiplier * timeDelta;
}

void PlayerHandler::ResetCollisionState() {
    playerInstance->creatureAttributes.collisionAttributes.collidedDown = false, playerInstance->creatureAttributes.collisionAttributes.collidedUp = false,
    playerInstance->creatureAttributes.collisionAttributes.collidedLeft = false, playerInstance->creatureAttributes.collisionAttributes.collidedRight = false;
    playerInstance->creatureAttributes.collisionAttributes.isAbovePlatform = false;
}

void PlayerHandler::HandleVerticalCollision(SDL_Rect entityRect, const LevelItem& levelItem,
                                            float timeDelta, float timeMultiplier) {
    float levelItemTop = float(levelItem.pos.y),
          levelItemBottom = float(levelItem.pos.y + levelItem.rect.h);

    bool hitFeet = IsSideColliding(entityRect, levelItem.rect, Direction::DOWN, playerInstance->creatureAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitFeet) {
        playerInstance->creatureAttributes.collisionAttributes.collidedDown = true;
        playerInstance->creatureAttributes.collisionAttributes.surfaceAttrition = levelItem.attritionCoefficient;
        playerInstance->creatureAttributes.positionNow.y = levelItemTop - float(playerInstance->creatureAttributes.model.h);
        playerInstance->creatureAttributes.velocityNow.y = 0;
        if (levelItem.collisionType == PLATFORM) playerInstance->creatureAttributes.collisionAttributes.isAbovePlatform = true;
    }

    if (levelItem.collisionType == CollisionType::PLATFORM) return;

    bool hitHead = IsSideColliding(entityRect, levelItem.rect, Direction::UP, playerInstance->creatureAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitHead) {
        playerInstance->creatureAttributes.collisionAttributes.collidedUp = true;
        playerInstance->creatureAttributes.positionNow.y = levelItemBottom;
        playerInstance->creatureAttributes.velocityNow.y = -playerInstance->creatureAttributes.velocityNow.y * SURFACE_BOUNCE;
    }
}

void PlayerHandler::HandleHorizontalCollision(SDL_Rect model, const LevelItem& levelItem,
                                              float timeDelta, float timeMultiplier) {
    if (levelItem.collisionType == PLATFORM) return;

    float levelItemLeft = float(levelItem.pos.x),
          levelItemRight = float(levelItem.pos.x + levelItem.rect.w);

    bool hitRight = IsSideColliding(model, levelItem.rect, Direction::RIGHT, playerInstance->creatureAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitRight) {
        playerInstance->creatureAttributes.collisionAttributes.collidedRight = true;
        playerInstance->creatureAttributes.velocityNow.x = -playerInstance->creatureAttributes.velocityNow.x * SURFACE_BOUNCE;
        playerInstance->creatureAttributes.positionNow.x = levelItemLeft - float(playerInstance->creatureAttributes.model.w);
    }

    bool hitLeft = IsSideColliding(model, levelItem.rect, Direction::LEFT, playerInstance->creatureAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitLeft) {
        playerInstance->creatureAttributes.collisionAttributes.collidedLeft = true;
        playerInstance->creatureAttributes.velocityNow.x = -playerInstance->creatureAttributes.velocityNow.x * SURFACE_BOUNCE;
        playerInstance->creatureAttributes.positionNow.x = levelItemRight;
    }
}

void PlayerHandler::UpdateModel() {
    playerInstance->creatureAttributes.model = SDL_Rect{
        .x = int(playerInstance->creatureAttributes.positionNow.x),
        .y = int(playerInstance->creatureAttributes.positionNow.y),
        .w = playerInstance->creatureAttributes.model.w,
        .h = playerInstance->creatureAttributes.model.h,
    };
}

void PlayerHandler::HandleCollisions(float timeDelta, float timeMultiplier, bool isPaused) {
    if (isPaused) return;

    ResetCollisionState();

    for (LevelItem levelItem : Level::collisions) {
        UpdateModel();
        HandleVerticalCollision(playerInstance->creatureAttributes.model, levelItem, timeDelta, timeMultiplier);  // TODO remove model
        UpdateModel();
        HandleHorizontalCollision(playerInstance->creatureAttributes.model, levelItem, timeDelta, timeMultiplier);
    }
}

void PlayerHandler::Draw(Vec2<int> cameraPos) {
    Vec2<float> positionNow = playerInstance->creatureAttributes.positionNow;

    SDL_Rect playerModel = {
        (int)positionNow.x - cameraPos.x,
        (int)positionNow.y - cameraPos.y,
        playerInstance->creatureAttributes.model.w,
        playerInstance->creatureAttributes.model.h,
    };
    scc(SDL_SetRenderDrawColor(renderer, RED, 0xff)).Handle();
    scc(SDL_RenderFillRect(renderer, &playerModel)).Handle();
}

void PlayerHandler::DrawSightLine(Vec2<int> cameraPos, SDL_Renderer* renderer) {
    float lineLength = 80;

    Vec2<int> lineBegin = {
        int(playerInstance->creatureAttributes.positionNow.x - cameraPos.x + playerInstance->creatureAttributes.model.w * 0.5f),
        int(playerInstance->creatureAttributes.positionNow.y - cameraPos.y + playerInstance->creatureAttributes.model.h * 0.5f),
    };

    Vec2<int> lineEnd = GetSightLineEnd(playerInstance->creatureAttributes.positionNow, playerInstance->creatureAttributes.model, playerInstance->angleFacing, cameraPos, lineLength);

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
