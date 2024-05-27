#include "../../include/entities/Player.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <cmath>

#include "../../include/entities/Camera.hh"
#include "../../include/main/Level.hh"
#include "../../lib/utils/engine_utils.hh"
#include "../../lib/utils/math_utils.hh"
#include "../../lib/utils/sdl_utils.hh"

//------------------------------------------------------------------------------

Player* Player::playerInstance = new Player;

Vector2<float> Player::GetPos() { return pos; }
Vector2<float> Player::GetVelocity() { return velocity; }
Player* Player::GetPlayerInstace() { return Player::playerInstance; }

Vector2<int> Player::GetHitboxInfo() { return hitbox; }

void Player::Handle(const float& delta, const float& timeMultiplier, const bool& isPaused) {
    HandleVelocity(delta, timeMultiplier, isPaused);
    HandleColisions(delta, timeMultiplier, isPaused);
}

void Player::HandleVelocity(const float& delta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused) return;
    Uint32 timeNow = SDL_GetTicks();

    {
        if (velocity.y > MAX_Y_SPEED) {
            velocity.y = MAX_Y_SPEED;
        }
        if (velocity.y < MIN_Y_SPEED) {
            velocity.y = MIN_Y_SPEED;
        }

        if (velocity.x > MAX_X_SPEED) {
            velocity.x = MAX_X_SPEED;
        }
        if (velocity.x < MIN_X_SPEED) {
            velocity.x = MIN_X_SPEED;
        }
    }

    if (colidedDown == false) {
        velocity.y += delta * GRAVITY * timeMultiplier;
        pos.y += velocity.y * delta * timeMultiplier;
    }

    if (!colidedDown) attrition = 0.8;  // resets attrition

    velocity.x -= delta * attrition * velocity.x * timeMultiplier;
    pos.x += velocity.x * timeMultiplier;
}

void Player::HandleColisions(const float& delta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused) return;
    (void)timeMultiplier;  // just so the compilers doesnt bitch about it
    colidedUp = false;
    colidedLeft = false;
    colidedRight = false;
    colidedDown = false;

    isAbovePlatform = false;

    float headOfPlayer = pos.y, leftOfPlayer = pos.x, rightOfPlayer = pos.x + hitbox.x,
          feetOfPLayer = pos.y + hitbox.y;

    bool isHorizontallyOverlaped, hitFeet, hitHead, isVerticallyOverlaped, hitRight, hitLeft;

    float colItemTop, colItemLeft, colItemRight, colItemBottom;

    for (LevelItem colisionItem : Level::colisions) {
        {
            colItemTop = colisionItem.pos.y;
            colItemBottom = colisionItem.pos.y + colisionItem.wireframe.h;
            colItemLeft = colisionItem.pos.x;
            colItemRight = colisionItem.pos.x + colisionItem.wireframe.w;
        }

        {
            isHorizontallyOverlaped = (rightOfPlayer > colItemLeft && rightOfPlayer < colItemRight) || (leftOfPlayer > colItemLeft && leftOfPlayer < colItemRight);

            hitFeet = feetOfPLayer + delta * velocity.y * timeMultiplier >= colItemTop &&  // 0.8 is the maximum that i've
                      feetOfPLayer <= colItemBottom - colisionItem.wireframe.h * 0.8 &&    // found not to break colision,
                      isHorizontallyOverlaped;                                             // this makes it so the player only
                                                                                           // goes up if above 20% o the
                                                                                           // colItem's height

            hitHead = headOfPlayer + delta * velocity.y * timeMultiplier <= colItemBottom &&
                      headOfPlayer >= colItemTop + colisionItem.wireframe.h * 0.9 &&
                      isHorizontallyOverlaped;
        }

        if (hitFeet) {
            colidedDown = true;
            attrition = colisionItem.attritionCoefficient;
            pos.y = colItemTop - hitbox.y;
            velocity.y = 0;
            if (colisionItem.colisionType == PLATFORM) isAbovePlatform = true;
        }

        if (colisionItem.colisionType != PLATFORM) {
            if (hitHead) {
                colidedUp = true;
                pos.y = colItemBottom;
                velocity.y = -velocity.y * 0.2f;
            }
            {  // this needs to be recalculated after changing the players pos
               // in hitFeet and/or hitHead
                headOfPlayer = pos.y, leftOfPlayer = pos.x, rightOfPlayer = pos.x + hitbox.x,
                feetOfPLayer = pos.y + hitbox.y;

                isVerticallyOverlaped = ((headOfPlayer > colItemTop && headOfPlayer < colItemBottom) ||
                                         (feetOfPLayer > colItemTop && feetOfPLayer < colItemBottom));

                hitRight = rightOfPlayer - velocity.x * delta * attrition * timeMultiplier >= colItemLeft &&
                           rightOfPlayer <= colItemRight &&
                           isVerticallyOverlaped;

                hitLeft = leftOfPlayer - velocity.x * delta * attrition * timeMultiplier <= colItemRight &&
                          leftOfPlayer >= colItemLeft &&
                          isVerticallyOverlaped;
            }

            if (hitRight) {
                colidedRight = true;
                velocity.x = -velocity.x * 0.2;
                pos.x = colItemLeft - hitbox.x;
            }
            if (hitLeft) {
                colidedLeft = true;
                velocity.x = -velocity.x * 0.2;
                pos.x = colItemRight;
            }
        }
    }
}

void Player::Draw(const Vector2<int>& cameraPos, SDL_Renderer* renderer) {
    SDL_Rect playerModel = {
        (int)pos.x - cameraPos.x,
        (int)pos.y - cameraPos.y,
        playerInstance->hitbox.x,
        playerInstance->hitbox.y,
    };
    scc(SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255));
    scc(SDL_RenderFillRect(renderer, &playerModel));
}

void Player::Move(const Directions& direction, const bool& isPaused) {
    if (isPaused) return;
    switch (direction) {
        case LEFT: {
            if (!colidedLeft) {
                velocity.x -= accelSpeed.x;
            }
            facing = LEFT;
            break;
        }
        case RIGHT: {
            if (!colidedRight) {
                velocity.x += accelSpeed.x;
            }
            facing = RIGHT;
            break;
        }
        case UP: {
            if (colidedDown && !colidedUp) {
                velocity.y -= accelSpeed.y;
            }
            break;
        }
        case DOWN: {
            if (isAbovePlatform) {
                pos.y += 6;               // 6 is the safest i've found given the height
                colidedDown = false;      // of the plaform in Engine.cc. This needs to be done
                isAbovePlatform = false;  // to place the player bellow the platform's top
            }  // TODO: change how this works
            break;
        }
        default: {
            break;
        };
    }
}

void Player::PrepareToDash(const Directions& direction, const float& startTick, SDL_Renderer* renderer, float* timeMultiplier) {
    angleDash = fmod(angleDash, 360);

    // slowly decrease timeMultiplier
    if (*timeMultiplier > 0.1) {
        *timeMultiplier += (0.1 - *timeMultiplier) / 25;
    }
    switch (direction) {
        // each angle position represents an angle
        case NONE: {
            break;
        }
        case LEFT: {
            angleDash += (180 - angleDash) / 20.0;
            break;
        }
        case RIGHT: {
            float addDash = (0 - angleDash) / 20.0f;
            if (angleDash > 180) {
                addDash = (360 - angleDash) / 20.0f;
            }
            angleDash += addDash;
            break;
        }
        case UP: {  // SDL flips "up" and "down"
            float addDash = (270 - angleDash) / 20.0f;
            if (angleDash > -90 && angleDash < 90) {
                addDash = (-90 - angleDash) / 20.0f;
            }
            angleDash += addDash;
            break;
        }
        case DOWN: {
            float addDash = (90 - angleDash) / 20.0f;
            if (angleDash > 270) {
                addDash = (450 - angleDash) / 20.0f;
            }
            angleDash += addDash;
            break;
        }
    }
    float angleDashInRadians = DegreesToRadians(angleDash);

    int lineDistance = 100;
    Vector2<int> pointStart = {(int)pos.x + hitbox.x / 2, (int)pos.y + hitbox.y / 2};
    Vector2<int> pointEnd = {int(pointStart.x + lineDistance * cos(angleDashInRadians)),
                             int(pointStart.y + lineDistance * sin(angleDashInRadians))};

    Vector2<float> cameraPos = Camera::pos;
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLine(renderer, pointStart.x - cameraPos.x, pointStart.y - cameraPos.y,
                       pointEnd.x - cameraPos.x, pointEnd.y - cameraPos.y);
    (void)startTick;
}

void Player::Dash() {
    Vector2<float> dashStrenght = {300.0f, 300.0f};
    float dashAngleInRadians = DegreesToRadians(angleDash);
    Vector2<float> dashVel = {dashStrenght.x * cos(dashAngleInRadians),
                              dashStrenght.y * sin(dashAngleInRadians)};
    velocity.x = dashVel.x;
    velocity.y = dashVel.y;
    isPreparingToDash = false;
}

//------------------------------------------------------------------------------
