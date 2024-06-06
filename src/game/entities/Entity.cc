#include "../../../include/game/entities/Entity.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <iostream>
#include <vector>

#include "../../../include/main/Level.hh"
#include "../../../lib/utils/sdl_utils.hh"

Entity::Entity(EntityID entityType_, Vec2<float> spawnPosition, Vec2<int> hitbox_) : ID(entityType_), positionNow(spawnPosition), hitbox(hitbox_) {
    entityVector.push_back(this);
}

Entity::~Entity() {}

std::vector<Entity*> Entity::entityVector = {};

bool Entity::GetColidedInformation(Direction direction) {
    switch (direction) {
        case Direction::UP: {
            return colidedUp;
        }
        case Direction::LEFT: {
            return colidedUp;
        }
        case Direction::RIGHT: {
            return colidedUp;
        }
        case Direction::DOWN: {
            return colidedUp;
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
    if (GetColidedInformation(direction)) return;
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
            if (!colidedDown) break;
            velocityNow.y -= accelSpeed.y;
            break;
        }
        case Direction::DOWN: {
            positionNow.y += 6;       // 6 is the safest i've found given the height
            colidedDown = false;      // of the plaform in Engine.cc. This needs to be done
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
    HandleColisions(timeDelta, timeMultiplier, isPaused);
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

        if (entity->colidedDown == false) {
            entity->velocityNow.y += timeDelta * GRAVITY * timeMultiplier;
            entity->positionNow.y += entity->velocityNow.y * timeDelta * timeMultiplier;
        }

        if (!entity->colidedDown) entity->surfaceAttrition = 0.8f;  // resets attrition

        entity->velocityNow.x -= timeDelta * entity->surfaceAttrition * entity->velocityNow.x * timeMultiplier;
        entity->positionNow.x += entity->velocityNow.x * timeMultiplier;
    }
}

void Entity::HandleColisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused) return;
    (void)timeMultiplier;  // just so the compilers doesnt bitch about it
    for (Entity* entity : entityVector) {
        entity->colidedUp = false;
        entity->colidedLeft = false;
        entity->colidedRight = false;
        entity->colidedDown = false;

        entity->isAbovePlatform = false;

        float headOfPlayer = entity->positionNow.y,
              leftOfPlayer = entity->positionNow.x,
              rightOfPlayer = entity->positionNow.x + float(entity->hitbox.x),
              feetOfPLayer = entity->positionNow.y + float(entity->hitbox.y);

        bool isHorizontallyOverlaped, hitFeet, hitHead,
            isVerticallyOverlaped, hitRight, hitLeft;

        float colItemTop,
            colItemLeft,
            colItemRight,
            colItemBottom;

        for (LevelItem colisionItem : Level::colisions) {
            {
                colItemTop = float(colisionItem.pos.y);
                colItemBottom = float(colisionItem.pos.y + colisionItem.wireframe.h);
                colItemLeft = float(colisionItem.pos.x);
                colItemRight = float(colisionItem.pos.x + colisionItem.wireframe.w);
            }

            {
                isHorizontallyOverlaped = (rightOfPlayer > colItemLeft &&
                                           rightOfPlayer < colItemRight) ||
                                          (leftOfPlayer > colItemLeft &&
                                           leftOfPlayer < colItemRight);

                hitFeet = feetOfPLayer + timeDelta * entity->velocityNow.y * timeMultiplier >= colItemTop &&  // 0.8 is the maximum that i've
                          feetOfPLayer <= colItemBottom - float(colisionItem.wireframe.h) * 0.8f &&           // found not to break colision,
                          isHorizontallyOverlaped;                                                            // this makes it so the player only
                                                                                                              // goes up if above 20% o the
                                                                                                              // colItem's height

                hitHead = headOfPlayer + timeDelta * entity->velocityNow.y * timeMultiplier <= colItemBottom &&
                          headOfPlayer >= colItemTop + float(colisionItem.wireframe.h) * 0.9f &&
                          isHorizontallyOverlaped;
            }

            if (hitFeet) {
                entity->colidedDown = true;
                entity->surfaceAttrition = colisionItem.attritionCoefficient;
                entity->positionNow.y = colItemTop - float(entity->hitbox.y);
                entity->velocityNow.y = 0;
                if (colisionItem.colisionType == PLATFORM) entity->isAbovePlatform = true;
            }

            if (colisionItem.colisionType != PLATFORM) {
                if (hitHead) {
                    entity->colidedUp = true;
                    entity->positionNow.y = colItemBottom;
                    entity->velocityNow.y = -entity->velocityNow.y * 0.2f;
                }
                {  // this needs to be recalculated after changing the players pos
                   // in hitFeet and/or hitHead
                    headOfPlayer = entity->positionNow.y, leftOfPlayer = entity->positionNow.x, rightOfPlayer = entity->positionNow.x + float(entity->hitbox.x),
                    feetOfPLayer = entity->positionNow.y + float(entity->hitbox.y);

                    isVerticallyOverlaped = ((headOfPlayer > colItemTop && headOfPlayer < colItemBottom) ||
                                             (feetOfPLayer > colItemTop && feetOfPLayer < colItemBottom));

                    hitRight = rightOfPlayer - entity->velocityNow.x * timeDelta * entity->surfaceAttrition * timeMultiplier >= colItemLeft &&
                               rightOfPlayer <= colItemRight &&
                               isVerticallyOverlaped;

                    hitLeft = leftOfPlayer - entity->velocityNow.x * timeDelta * entity->surfaceAttrition * timeMultiplier <= colItemRight &&
                              leftOfPlayer >= colItemLeft &&
                              isVerticallyOverlaped;
                }

                if (hitRight) {
                    entity->colidedRight = true;
                    entity->velocityNow.x = -entity->velocityNow.x * 0.2f;
                    entity->positionNow.x = colItemLeft - float(entity->hitbox.x);
                }
                if (hitLeft) {
                    entity->colidedLeft = true;
                    entity->velocityNow.x = -entity->velocityNow.x * 0.2f;
                    entity->positionNow.x = colItemRight;
                }
            }
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
