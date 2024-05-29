#include "../../include/entities/Entity.hh"

#include <SDL2/SDL.h>

#include <vector>

#include "../../include/main/Level.hh"
#include "../../lib/utils/sdl_utils.hh"

Entity::Entity(EntityID entityType_, Vector2<float> accelSpeed_, Vector2<float> spawnPosition, Vector2<int> hitbox_) : ID(entityType_), accelSpeed(accelSpeed_), position(spawnPosition), hitbox(hitbox_) {
    entityVector.push_back(this);
}

Entity::~Entity() {}

std::vector<Entity*> Entity::entityVector = {};

bool Entity::GetColidedInformation(Direction direction) {
    switch (direction) {
        case UP: {
            return colidedUp;
        }
        case LEFT: {
            return colidedUp;
        }
        case RIGHT: {
            return colidedUp;
        }
        case DOWN: {
            return colidedUp;
        }
    }
    return false;
}

Vector2<float> Entity::GetVelocity() { return velocity; }
Vector2<float> Entity::GetPos() { return position; }

Vector2<int> Entity::GetHitbox() { return hitbox; }

void Entity::Move(const Direction& direction, const bool& isPaused) {
    if (isPaused) return;
    if (GetColidedInformation(direction)) return;
    switch (direction) {
        case LEFT: {
            velocity.x -= accelSpeed.x;
            facing = LEFT;
            break;
        }
        case RIGHT: {
            velocity.x += accelSpeed.x;
            facing = RIGHT;
            break;
        }
        case UP: {
            if (!colidedDown) break;
            velocity.y -= accelSpeed.y;
            break;
        }
        case DOWN: {
            position.y += 6;          // 6 is the safest i've found given the height
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

void Entity::Draw(const Vector2<int>& cameraPos, SDL_Renderer* renderer) {
    for (Entity* entity : entityVector) {
        SDL_Rect entityModel = {
            // for now, because I dont have proper models
            (int)entity->position.x - cameraPos.x,
            (int)entity->position.y - cameraPos.y,
            entity->hitbox.x,
            entity->hitbox.y,
        };
        SDL_Color color = SDL_Color{RED};
        if (entity->ID != PLAYER) color = SDL_Color{YELLOW};
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
            if (entity->velocity.y > MAX_Y_SPEED) {
                entity->velocity.y = MAX_Y_SPEED;
            }
            if (entity->velocity.y < MIN_Y_SPEED) {
                entity->velocity.y = MIN_Y_SPEED;
            }

            if (entity->velocity.x > MAX_X_SPEED) {
                entity->velocity.x = MAX_X_SPEED;
            }
            if (entity->velocity.x < MIN_X_SPEED) {
                entity->velocity.x = MIN_X_SPEED;
            }
        }

        if (entity->colidedDown == false) {
            entity->velocity.y += timeDelta * GRAVITY * timeMultiplier;
            entity->position.y += entity->velocity.y * timeDelta * timeMultiplier;
        }

        if (!entity->colidedDown) entity->surfaceAttrition = 0.8f;  // resets attrition

        entity->velocity.x -= timeDelta * entity->surfaceAttrition * entity->velocity.x * timeMultiplier;
        entity->position.x += entity->velocity.x * timeMultiplier;
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

        float headOfPlayer = entity->position.y,
              leftOfPlayer = entity->position.x,
              rightOfPlayer = entity->position.x + float(entity->hitbox.x),
              feetOfPLayer = entity->position.y + float(entity->hitbox.y);

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

                hitFeet = feetOfPLayer + timeDelta * entity->velocity.y * timeMultiplier >= colItemTop &&  // 0.8 is the maximum that i've
                          feetOfPLayer <= colItemBottom - float(colisionItem.wireframe.h) * 0.8f &&        // found not to break colision,
                          isHorizontallyOverlaped;                                                         // this makes it so the player only
                                                                                                           // goes up if above 20% o the
                                                                                                           // colItem's height

                hitHead = headOfPlayer + timeDelta * entity->velocity.y * timeMultiplier <= colItemBottom &&
                          headOfPlayer >= colItemTop + float(colisionItem.wireframe.h) * 0.9f &&
                          isHorizontallyOverlaped;
            }

            if (hitFeet) {
                entity->colidedDown = true;
                entity->surfaceAttrition = colisionItem.attritionCoefficient;
                entity->position.y = colItemTop - float(entity->hitbox.y);
                entity->velocity.y = 0;
                if (colisionItem.colisionType == PLATFORM) entity->isAbovePlatform = true;
            }

            if (colisionItem.colisionType != PLATFORM) {
                if (hitHead) {
                    entity->colidedUp = true;
                    entity->position.y = colItemBottom;
                    entity->velocity.y = -entity->velocity.y * 0.2f;
                }
                {  // this needs to be recalculated after changing the players pos
                   // in hitFeet and/or hitHead
                    headOfPlayer = entity->position.y, leftOfPlayer = entity->position.x, rightOfPlayer = entity->position.x + float(entity->hitbox.x),
                    feetOfPLayer = entity->position.y + float(entity->hitbox.y);

                    isVerticallyOverlaped = ((headOfPlayer > colItemTop && headOfPlayer < colItemBottom) ||
                                             (feetOfPLayer > colItemTop && feetOfPLayer < colItemBottom));

                    hitRight = rightOfPlayer - entity->velocity.x * timeDelta * entity->surfaceAttrition * timeMultiplier >= colItemLeft &&
                               rightOfPlayer <= colItemRight &&
                               isVerticallyOverlaped;

                    hitLeft = leftOfPlayer - entity->velocity.x * timeDelta * entity->surfaceAttrition * timeMultiplier <= colItemRight &&
                              leftOfPlayer >= colItemLeft &&
                              isVerticallyOverlaped;
                }

                if (hitRight) {
                    entity->colidedRight = true;
                    entity->velocity.x = -entity->velocity.x * 0.2f;
                    entity->position.x = colItemLeft - float(entity->hitbox.x);
                }
                if (hitLeft) {
                    entity->colidedLeft = true;
                    entity->velocity.x = -entity->velocity.x * 0.2f;
                    entity->position.x = colItemRight;
                }
            }
        }
    }
}
