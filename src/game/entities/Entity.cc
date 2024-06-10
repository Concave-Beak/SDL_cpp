#include "../../../include/game/entities/Entity.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <vector>

#include "../../../include/main/Level.hh"
#include "../../../lib/utils/sdl_utils.hh"

Entity::Entity(EntityID entityType_, Vec2<float> spawnPosition, Vec2<int> hitbox_) : ID(entityType_), positionNow(spawnPosition), hitbox(hitbox_) {
    entityVector.push_back(this);
}

Entity::~Entity() {}

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
    for (Entity* entity : entityVector) {
        SDL_Rect entityModel = {
            // rectangles for now, because I dont have proper models
            (int)entity->positionNow.x - cameraPos.x,
            (int)entity->positionNow.y - cameraPos.y,
            entity->hitbox.x,
            entity->hitbox.y,
        };
        SDL_Color color = SDL_Color{RED, 0xff};
        if (entity->ID != PLAYER) color = SDL_Color{YELLOW, 0xff};
        scc(SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a));
        scc(SDL_RenderFillRect(renderer, &entityModel));
    }
}

void Entity::Handle(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) {
    HandleVelocity(timeDelta, timeMultiplier, isPaused);
    HandleCollisions(timeDelta, timeMultiplier, isPaused);
}

void Entity::HandleVelocity(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused) return;

    for (Entity* entity : entityVector) {
        {
            if (entity->velocityNow.y > MAX_Y_SPEED) {
                entity->velocityNow.y = MAX_Y_SPEED;
            }
            if (entity->velocityNow.y < MIN_Y_SPEED) {
                entity->velocityNow.y = MIN_Y_SPEED;
            }

            if (entity->velocityNow.x > MAX_X_SPEED) {
                entity->velocityNow.x = MAX_X_SPEED;
            }
            if (entity->velocityNow.x < MIN_X_SPEED) {
                entity->velocityNow.x = MIN_X_SPEED;
            }
        }

        if (entity->collidedDown == false) {
            entity->velocityNow.y += timeDelta * GRAVITY * timeMultiplier;
            entity->positionNow.y += entity->velocityNow.y * timeDelta * timeMultiplier;
        }

        if (!entity->collidedDown) entity->surfaceAttrition = 0.8f;  // resets attrition

        entity->velocityNow.x -= timeDelta * entity->surfaceAttrition * entity->velocityNow.x * timeMultiplier;
        entity->positionNow.x += entity->velocityNow.x * timeMultiplier;
    }
}

void Entity::ResetCollisionState() {
    collidedUp = false;
    collidedLeft = false;
    collidedRight = false;
    collidedDown = false;
    isAbovePlatform = false;
}

SDL_Rect Entity::GetEntityRect(const Entity& entity) {
    return SDL_Rect{
        .x = int(entity.positionNow.x),
        .y = int(entity.positionNow.y),
        .w = entity.hitbox.x,
        .h = entity.hitbox.y,
    };
}

void Entity::HandleVerticalCollision(Entity* entity, const SDL_Rect& entityRect,
                                     const LevelItem& levelItem,
                                     const float& timeDelta, const float& timeMultiplier) {
    float levelItemTop = float(levelItem.pos.y), levelItemBottom = float(levelItem.pos.y + levelItem.wireframe.h);

    bool hitFeet = CheckSideCollision(entityRect, levelItem.wireframe, Direction::DOWN, entity->velocityNow, timeDelta, timeMultiplier);
    if (hitFeet) {
        entity->collidedDown = true;
        entity->surfaceAttrition = levelItem.attritionCoefficient;
        entity->positionNow.y = levelItemTop - float(entity->hitbox.y);
        entity->velocityNow.y = 0;
        if (levelItem.collisionType == PLATFORM) entity->isAbovePlatform = true;
    }

    if (levelItem.collisionType == CollisionType::PLATFORM) return;

    bool hitHead = CheckSideCollision(entityRect, levelItem.wireframe, Direction::UP, entity->velocityNow, timeDelta, timeMultiplier);
    if (hitHead) {
        entity->collidedUp = true;
        entity->positionNow.y = levelItemBottom;
        entity->velocityNow.y = -entity->velocityNow.y * 0.2f;
    }
}

void Entity::HandleHorizontalCollision(Entity* entity, const SDL_Rect& entityRect,
                                       const LevelItem& levelItem,
                                       const float& timeDelta, const float& timeMultiplier) {
    if (levelItem.collisionType == PLATFORM) return;

    float levelItemLeft = float(levelItem.pos.x);
    float levelItemRight = float(levelItem.pos.x + levelItem.wireframe.w);

    bool hitRight = CheckSideCollision(entityRect, levelItem.wireframe, Direction::RIGHT, entity->velocityNow, timeDelta, timeMultiplier);
    if (hitRight) {
        entity->collidedRight = true;
        entity->velocityNow.x = -entity->velocityNow.x * 0.2f;
        entity->positionNow.x = levelItemLeft - float(entity->hitbox.x);
    }

    bool hitLeft = CheckSideCollision(entityRect, levelItem.wireframe, Direction::LEFT, entity->velocityNow, timeDelta, timeMultiplier);
    if (hitLeft) {
        entity->collidedLeft = true;
        entity->velocityNow.x = -entity->velocityNow.x * 0.2f;
        entity->positionNow.x = levelItemRight;
    }
}

void Entity::HandleCollisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused) return;

    for (Entity* entity : entityVector) {
        entity->ResetCollisionState();

        for (LevelItem levelItem : Level::collisions) {
            SDL_Rect entityRect = GetEntityRect(*entity);
            HandleVerticalCollision(entity, entityRect, levelItem, timeDelta, timeMultiplier);
            entityRect = GetEntityRect(*entity);
            HandleHorizontalCollision(entity, entityRect, levelItem, timeDelta, timeMultiplier);
        }
    }
}

//------------------------------------------------------------------------------

Attack::Attack(Uint32 damage_, Vec2<float> spawnPos_, AttackType atkType_, Uint32 lifeTime) : damage(damage_), spawnPos(spawnPos_), atkType(atkType_), lifeEndTick(SDL_GetTicks() + lifeTime), hitbox({30, 30}) {
    switch (atkType_) {
        case AttackType::ARROW: {
            this->hitbox = {50, 10};
            break;
        }
        case AttackType::SWORD_SLASH: {
            this->hitbox = {30, 20};
            break;
        }
        default: {
            this->hitbox = {30, 10};
            break;
        }
    }
    attackVector.push_back(this);
}

Attack::~Attack() {
}

void Attack::Draw(const Vec2<float>& cameraPos, SDL_Renderer* renderer) {
    for (Attack* attack : attackVector) {
        SDL_Rect attackModel = {
            int(attack->spawnPos.x - cameraPos.x),
            int(attack->spawnPos.y - cameraPos.y),
            attack->hitbox.x,
            attack->hitbox.y,
        };
        SDL_Color color = SDL_Color{BLACK, 0xff};
        scc(SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a));
        scc(SDL_RenderFillRect(renderer, &attackModel));
    }
}

void Attack::CheckAndDestroyExpiredAttacks() {
    Uint32 currentTicks = SDL_GetTicks();
    for (std::vector<Attack*>::iterator it = attackVector.begin(); it != attackVector.end();) {
        if ((*it)->lifeEndTick <= currentTicks) {
            delete *it;
            it = attackVector.erase(it);
        } else {
            ++it;
        }
    }
}
