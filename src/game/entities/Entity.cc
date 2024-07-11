#include "../../../include/game/entities/Entity.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <memory>
#include <vector>

#include "../../../include/main/Level.hh"
#include "../../../lib/utils/sdl_utils.hh"

std::shared_ptr<Entity> Entity::Create() {
    std::shared_ptr<Entity> entity(new Entity());
    entityVector.push_back(entity);
    return entity;
}

bool Entity::GetCollidedInformation(Direction direction) {
    switch (direction) {
        case Direction::UP: {
            return collidedUp;
        }
        case Direction::LEFT: {
            return collidedUp;
        }
        case Direction::RIGHT: {
            return collidedUp;
        }
        case Direction::DOWN: {
            return collidedUp;
        }
    }
    return false;
}

Vec2<float> Entity::GetVelocityNow() { return velocityNow; }
Vec2<float> Entity::GetPos() { return positionNow; }

SDL_Rect Entity::GetModel() {
    UpdateModel();
    return model;
}

Direction Entity::GetFacingDirection() { return facing; }

void Entity::Move(const Direction& direction, const Vec2<float>& accelSpeed, const bool& isPaused) {
    if (isPaused) return;
    if (GetCollidedInformation(direction)) return;
    switch (direction) {
        case Direction::LEFT: {
            velocityNow.x -= accelSpeed.x;
            facing = Direction::LEFT;
            break;
        }
        case Direction::RIGHT: {
            velocityNow.x += accelSpeed.x;
            facing = Direction::RIGHT;
            break;
        }
        case Direction::UP: {
            if (!collidedDown) break;
            velocityNow.y -= accelSpeed.y;
            collidedDown = false;
            break;
        }
        case Direction::DOWN: {
            if (!isAbovePlatform) return;
            positionNow.y += 6;       // 6 is the safest i've found given the height
            collidedDown = false;     // of the plaform in Engine.cc. This needs to be done
            isAbovePlatform = false;  // to place the player bellow the platform's top
                                      // TODO: change how this works
            break;
        }
        default: {
            break;
        };
    }
}

void Entity::Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer) {
    SDL_Rect modelToDraw = model - SDL_Rect{.x = cameraPos.x, .y = cameraPos.y, .w = 0, .h = 0};
    scc(SDL_SetRenderDrawColor(renderer, PINK, 0xff)).Handle();
    scc(SDL_RenderFillRect(renderer, &modelToDraw)).Handle();
}

void Entity::CheckExpiredEntities() {
    for (entityVec::iterator entityIt = entityVector.begin(); entityIt != entityVector.end();) {
        if ((*entityIt)->isMarkedForDeletion) {
            Delete(*entityIt);
        } else {
            ++entityIt;
        }
    }
}

void Entity::Delete(std::shared_ptr<Entity> entityIt) {
    entityVector.erase(std::remove(entityVector.begin(), entityVector.end(), entityIt), entityVector.end());
}

void Entity::Handle(const float& timeDelta, const float& timeMultiplier, const bool& isPaused, const Vec2<int>& cameraPos, SDL_Renderer* renderer) {
    CheckExpiredEntities();
    for (std::weak_ptr<Entity> entity : entityVector) {
        if (entity.expired()) continue;

        entity.lock()->HandleVelocity(timeDelta, timeMultiplier, isPaused);
        entity.lock()->HandleCollisions(timeDelta, timeMultiplier, isPaused);
        entity.lock()->Draw(cameraPos, renderer);
    }
}

void Entity::HandleVelocity(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused) return;

    if (velocityNow.y > MAX_Y_SPEED) {
        velocityNow.y = MAX_Y_SPEED;
    }
    if (velocityNow.y < -MAX_Y_SPEED) {
        velocityNow.y = -MAX_Y_SPEED;
    }

    if (velocityNow.x > MAX_X_SPEED) {
        velocityNow.x = MAX_X_SPEED;
    }
    if (velocityNow.x < -MAX_X_SPEED) {
        velocityNow.x = -MAX_X_SPEED;
    }

    if (!collidedDown) {
        velocityNow.y += timeDelta * GRAVITY * timeMultiplier;
        positionNow.y += velocityNow.y * timeDelta * timeMultiplier;
    }

    if (!collidedDown) surfaceAttrition = AIR_ATTRITION;  // resets attrition

    velocityNow.x -= timeDelta * surfaceAttrition * timeMultiplier * velocityNow.x;
    positionNow.x += velocityNow.x * timeMultiplier * timeDelta;
}

void Entity::ResetCollisionState() {
    collidedUp = false;
    collidedLeft = false;
    collidedRight = false;
    collidedDown = false;
    isAbovePlatform = false;
}

void Entity::UpdateModel() {
    model = SDL_Rect{
        .x = int(positionNow.x),
        .y = int(positionNow.y),
        .w = model.w,
        .h = model.h,
    };
}

void Entity::PushToEntityVector(std::shared_ptr<Entity> entity2Push) {
    entityVector.push_back(entity2Push);
}

std::vector<std::shared_ptr<Entity>> Entity::GetEntities() { return entityVector; }

Entity* Entity::GetEntity() { return this; }

Entity::EntityType Entity::GetType() { return type; }

void Entity::HandleVerticalCollision(const SDL_Rect& entityRect, const LevelItem& levelItem,
                                     const float& timeDelta, const float& timeMultiplier) {
    float levelItemTop = float(levelItem.pos.y),
          levelItemBottom = float(levelItem.pos.y + levelItem.rect.h);

    bool hitFeet = IsSideColliding(entityRect, levelItem.rect, Direction::DOWN, velocityNow, timeDelta, timeMultiplier);
    if (hitFeet) {
        collidedDown = true;
        surfaceAttrition = levelItem.attritionCoefficient;
        positionNow.y = levelItemTop - float(model.h);
        velocityNow.y = 0;
        if (levelItem.collisionType == PLATFORM) isAbovePlatform = true;
    }

    if (levelItem.collisionType == CollisionType::PLATFORM) return;

    bool hitHead = IsSideColliding(entityRect, levelItem.rect, Direction::UP, velocityNow, timeDelta, timeMultiplier);
    if (hitHead) {
        collidedUp = true;
        positionNow.y = levelItemBottom;
        velocityNow.y = -velocityNow.y * SURFACE_BOUNCE;
    }
}

void Entity::HandleHorizontalCollision(const SDL_Rect& entityRect, const LevelItem& levelItem,
                                       const float& timeDelta, const float& timeMultiplier) {
    if (levelItem.collisionType == PLATFORM) return;

    float levelItemLeft = float(levelItem.pos.x),
          levelItemRight = float(levelItem.pos.x + levelItem.rect.w);

    bool hitRight = IsSideColliding(entityRect, levelItem.rect, Direction::RIGHT, velocityNow, timeDelta, timeMultiplier);
    if (hitRight) {
        collidedRight = true;
        velocityNow.x = -velocityNow.x * SURFACE_BOUNCE;
        positionNow.x = levelItemLeft - float(model.w);
    }

    bool hitLeft = IsSideColliding(entityRect, levelItem.rect, Direction::LEFT, velocityNow, timeDelta, timeMultiplier);
    if (hitLeft) {
        collidedLeft = true;
        velocityNow.x = -velocityNow.x * SURFACE_BOUNCE;
        positionNow.x = levelItemRight;
    }
}

void Entity::HandleCollisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused) return;

    ResetCollisionState();

    for (LevelItem levelItem : Level::collisions) {
        UpdateModel();
        HandleVerticalCollision(model, levelItem, timeDelta, timeMultiplier);
        UpdateModel();
        HandleHorizontalCollision(model, levelItem, timeDelta, timeMultiplier);
    }
}

void Entity::Damage(int damage) {
    heathNow -= damage;
    if (heathNow <= 0) isDead = true;
}

//------------------------------------------------------------------------------
