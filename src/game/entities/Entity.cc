#include "../../../include/game/entities/Entity.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <iostream>
#include <vector>

#include "../../../include/main/Level.hh"
#include "../../../lib/utils/sdl_utils.hh"

Entity::Entity() {
    this->ID = entityVector.size();
    entityVector.push_back(this);
}
Entity::~Entity() {}

void Entity::Init(EntityType ID_) {
    this->Type = ID_;
}

std::vector<Entity*> Entity::entityVector = {};

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

Vec2<int> Entity::GetHitbox() { return hitbox; }

Direction Entity::GetFacingDirection() { return facing; }

void Entity::Move(const Direction& direction, const Vec2<float>& accelSpeed, const bool& isPaused) {
    if (isPaused) return;
    if (GetCollidedInformation(direction)) return;
    switch (direction) {
        case Direction::LEFT: {
            velocityNow.x -= accelSpeed.x;
            facing = LEFT;
            break;
        }
        case Direction::RIGHT: {
            velocityNow.x += accelSpeed.x;
            facing = RIGHT;
            break;
        }
        case Direction::UP: {
            if (!collidedDown) break;
            velocityNow.y -= accelSpeed.y;
            break;
        }
        case Direction::DOWN: {
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
    SDL_Rect entityModel = {
        // rectangles for now, because I dont have proper models
        (int)this->positionNow.x - cameraPos.x,
        (int)this->positionNow.y - cameraPos.y,
        this->hitbox.x,
        this->hitbox.y,
    };
    scc(SDL_SetRenderDrawColor(renderer, PINK, 0xff));
    scc(SDL_RenderFillRect(renderer, &entityModel));
}

void Entity::Handle(const float& timeDelta, const float& timeMultiplier, const bool& isPaused, const Vec2<int>& cameraPos, SDL_Renderer* renderer) {
    for (Entity* entity : entityVector) {
        entity->HandleVelocity(timeDelta, timeMultiplier, isPaused);
        entity->HandleCollisions(timeDelta, timeMultiplier, isPaused);
        entity->Draw(cameraPos, renderer);
    }
}

void Entity::HandleVelocity(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused) return;

    if (this->velocityNow.y > MAX_Y_SPEED) {
        this->velocityNow.y = MAX_Y_SPEED;
    }
    if (this->velocityNow.y < -MAX_Y_SPEED) {
        this->velocityNow.y = -MAX_Y_SPEED;
    }

    if (this->velocityNow.x > MAX_X_SPEED) {
        this->velocityNow.x = MAX_X_SPEED;
    }
    if (this->velocityNow.x < -MAX_X_SPEED) {
        this->velocityNow.x = -MAX_X_SPEED;
    }

    if (!this->collidedDown) {
        this->velocityNow.y += timeDelta * GRAVITY * timeMultiplier;
        this->positionNow.y += this->velocityNow.y * timeDelta * timeMultiplier;
    }

    if (!this->collidedDown) this->surfaceAttrition = AIR_ATTRITION;  // resets attrition

    this->velocityNow.x -= timeDelta * this->surfaceAttrition * timeMultiplier * this->velocityNow.x;
    this->positionNow.x += this->velocityNow.x * timeMultiplier * timeDelta;
}

void Entity::ResetCollisionState() {
    collidedUp = false;
    collidedLeft = false;
    collidedRight = false;
    collidedDown = false;
    isAbovePlatform = false;
}

SDL_Rect Entity::GetEntityRect() {
    return SDL_Rect{
        .x = int(this->positionNow.x),
        .y = int(this->positionNow.y),
        .w = this->hitbox.x,
        .h = this->hitbox.y,
    };
}

Entity* Entity::GetEntity() { return this; }

void Entity::HandleVerticalCollision(const SDL_Rect& entityRect, const LevelItem& levelItem,
                                     const float& timeDelta, const float& timeMultiplier) {
    float levelItemTop = float(levelItem.pos.y),
          levelItemBottom = float(levelItem.pos.y + levelItem.wireframe.h);

    bool hitFeet = CheckSideCollision(entityRect, levelItem.wireframe,
                                      Direction::DOWN, this->velocityNow, timeDelta, timeMultiplier);
    if (hitFeet) {
        this->collidedDown = true;
        this->surfaceAttrition = levelItem.attritionCoefficient;
        this->positionNow.y = levelItemTop - float(this->hitbox.y);
        this->velocityNow.y = 0;
        if (levelItem.collisionType == PLATFORM) this->isAbovePlatform = true;
    }

    if (levelItem.collisionType == CollisionType::PLATFORM) return;

    bool hitHead = CheckSideCollision(entityRect, levelItem.wireframe,
                                      Direction::UP, this->velocityNow, timeDelta, timeMultiplier);
    if (hitHead) {
        this->collidedUp = true;
        this->positionNow.y = levelItemBottom;
        this->velocityNow.y = -this->velocityNow.y * SURFACE_BOUNCE;
    }
}

void Entity::HandleHorizontalCollision(const SDL_Rect& entityRect, const LevelItem& levelItem,
                                       const float& timeDelta, const float& timeMultiplier) {
    if (levelItem.collisionType == PLATFORM) return;

    float levelItemLeft = float(levelItem.pos.x),
          levelItemRight = float(levelItem.pos.x + levelItem.wireframe.w);

    bool hitRight = CheckSideCollision(entityRect, levelItem.wireframe,
                                       Direction::RIGHT, this->velocityNow, timeDelta, timeMultiplier);
    if (hitRight) {
        this->collidedRight = true;
        this->velocityNow.x = -this->velocityNow.x * SURFACE_BOUNCE;
        this->positionNow.x = levelItemLeft - float(this->hitbox.x);
    }

    bool hitLeft = CheckSideCollision(entityRect, levelItem.wireframe,
                                      Direction::LEFT, this->velocityNow, timeDelta, timeMultiplier);
    if (hitLeft) {
        this->collidedLeft = true;
        this->velocityNow.x = -this->velocityNow.x * SURFACE_BOUNCE;
        this->positionNow.x = levelItemRight;
    }
}

void Entity::HandleCollisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused) return;

    this->ResetCollisionState();

    SDL_Rect entityRect;
    for (LevelItem levelItem : Level::collisions) {
        entityRect = this->GetEntityRect();
        this->HandleVerticalCollision(entityRect, levelItem, timeDelta, timeMultiplier);
        entityRect = this->GetEntityRect();
        this->HandleHorizontalCollision(entityRect, levelItem, timeDelta, timeMultiplier);
    }
}

void Entity::Damage(int damage) {
    this->heathNow -= damage;
    if (heathNow <= 0) isDead = true;
}

//------------------------------------------------------------------------------
