#include "Engine.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "../../Include/Utils/Utils.hh"
#include "Level.hh"

//------------------------------------------------------------------------------

void ClearBackground(SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, NULL);
}

//------------------------------------------------------------------------------

void Engine::Loop() {
    float beginTick = 0;
    Vec2f *posPlayer = &playerInstance->pos;
    Vec2f *velPlayer = &playerInstance->velocity;
    Vec2i playerColisionboxInfo = playerInstance->GetHitboxInfo();

    new LevelItem(Vec2i{SCREEN_WIDTH / 2, SCREEN_HEIGHT - 130}, {100, 30}, PLATFORM, SDL_Color{0, 0xff, 0, 0xff}, WOOD);                                 // Placeholder
    new LevelItem(Vec2i{SCREEN_WIDTH / 2, SCREEN_HEIGHT - 430}, {100, 100}, FULL_COLISION, SDL_Color{0, 0xff, 0, 0xff}, STONE);                          // Placeholder
    new LevelItem(Vec2i{SCREEN_WIDTH / 4, SCREEN_HEIGHT - playerColisionboxInfo.y - 110}, {100, 100}, FULL_COLISION, SDL_Color{0, 0xff, 0, 0xff}, MUD);  // Placeholder
    new LevelItem(Vec2i{0, SCREEN_HEIGHT - 5}, {SCREEN_WIDTH, 40}, FULL_COLISION, SDL_Color{0, 0, 0xff, 0xff}, DIRT);                                    // Placeholder

    while (!quit) {
        beginTick = SDL_GetTicks();
        HandleVelocity(posPlayer, velPlayer, playerColisionboxInfo);
        {  // Rendering
            ClearBackground(renderer, 100, 100, 100, 255);
            if (debugMode) {
                ShowDebugInfo();
            }

            Render();
            HandleEvent(&event);
            HandleFPS(beginTick);
            SDL_RenderPresent(renderer);
            SDL_RenderClear(renderer);
        }
        // reset timeMultiplier
        if (timeMultiplier < 1 && !playerInstance->isPreparingToDash) {
            timeMultiplier += (1 - timeMultiplier) / 50;
        }
    }
}

void Engine::HandleVelocity(Vec2f *posPlayer, Vec2f *velPlayer, Vec2i playerColisionboxInfo) {
    Uint32 timeNow = SDL_GetTicks();

    float delta = (timeNow - lastUpdate) / 300.0f;  // 300 is just to make delta easier to work with

    float attritionCoefficient = 0;

    {
        if (velPlayer->y > MAX_Y_SPEED) {
            velPlayer->y = MAX_Y_SPEED;
        }
        if (velPlayer->y < MIN_Y_SPEED) {
            velPlayer->y = MIN_Y_SPEED;
        }

        if (velPlayer->x > MAX_X_SPEED) {
            velPlayer->x = MAX_X_SPEED;
        }
        if (velPlayer->x < MIN_X_SPEED) {
            velPlayer->x = MIN_X_SPEED;
        }
    }

    if (playerInstance->colidedDown == false) {
        velPlayer->y += delta * GRAVITY * timeMultiplier;
        posPlayer->y += velPlayer->y * delta * timeMultiplier;
    }

    if (!playerInstance->colidedDown) attritionCoefficient = 0.8;

    HandleColisions(posPlayer, velPlayer, playerColisionboxInfo, delta, &attritionCoefficient, timeMultiplier);

    velPlayer->x -= delta * attritionCoefficient * velPlayer->x * timeMultiplier;
    posPlayer->x += velPlayer->x * timeMultiplier;

    camera->FollowPlayer(*posPlayer, delta, {SCREEN_WIDTH, SCREEN_HEIGHT}, playerColisionboxInfo, timeMultiplier);

    lastUpdate = SDL_GetTicks();
}

void Engine::HandleColisions(Vec2f *posPlayer, Vec2f *velPlayer, const Vec2i colisionBoxPlayer, const float delta, float *attritionCoefficient, const float timeMultipler) {
    playerInstance->colidedUp = false;
    playerInstance->colidedLeft = false;
    playerInstance->colidedRight = false;
    playerInstance->colidedDown = false;

    playerInstance->isAbovePlatform = false;

    float headOfPlayer = posPlayer->y,
          leftOfPlayer = posPlayer->x,
          rightOfPlayer = posPlayer->x + colisionBoxPlayer.x,
          feetOfPLayer = posPlayer->y + colisionBoxPlayer.y;

    bool isHorizontallyOverlaped, hitFeet, hitHead,
        isVerticallyOverlaped, hitRight, hitLeft;

    float colItemTop,
        colItemLeft,
        colItemRight,
        colItemBottom;

    for (LevelItem colisionItem : Level::colisions) {
        {
            colItemTop = colisionItem.pos.y;
            colItemBottom = colisionItem.pos.y + colisionItem.wireframe.h;
            colItemLeft = colisionItem.pos.x;
            colItemRight = colisionItem.pos.x + colisionItem.wireframe.w;
        }

        {
            isHorizontallyOverlaped = (rightOfPlayer > colItemLeft && rightOfPlayer < colItemRight) ||
                                      (leftOfPlayer > colItemLeft && leftOfPlayer < colItemRight);

            hitFeet = feetOfPLayer + delta * velPlayer->y * timeMultiplier >= colItemTop &&
                      feetOfPLayer <= colItemBottom - colisionItem.wireframe.h * 0.8 &&  // 0.8 is the maximum that i've found not to break colision,
                      isHorizontallyOverlaped;                                           // this makes it so the player only goes up if above 20% o the colItem's height

            hitHead = headOfPlayer + delta * velPlayer->y * timeMultiplier <= colItemBottom &&
                      headOfPlayer >= colItemTop + colisionItem.wireframe.h * 0.9 &&
                      isHorizontallyOverlaped &&
                      colisionItem.colisionType != PLATFORM;
        }

        if (hitFeet) {
            playerInstance->colidedDown = true;
            *attritionCoefficient = colisionItem.attritionCoefficient;
            posPlayer->y = colItemTop - colisionBoxPlayer.y;
            velPlayer->y = 0;
            if (colisionItem.colisionType == PLATFORM) playerInstance->isAbovePlatform = true;
        }

        if (colisionItem.colisionType != PLATFORM) {
            if (hitHead) {
                playerInstance->colidedUp = true;
                posPlayer->y = colItemBottom;
                velPlayer->y = -velPlayer->y * 0.2f;
            }
            {  // this needs to be recalculated after changing the players pos in hitFeet and/or hitHead
                headOfPlayer = posPlayer->y,
                leftOfPlayer = posPlayer->x,
                rightOfPlayer = posPlayer->x + colisionBoxPlayer.x,
                feetOfPLayer = posPlayer->y + colisionBoxPlayer.y;

                isVerticallyOverlaped = ((headOfPlayer > colItemTop && headOfPlayer < colItemBottom) ||
                                         (feetOfPLayer > colItemTop && feetOfPLayer < colItemBottom));

                hitRight = rightOfPlayer - velPlayer->x * delta * *(attritionCoefficient)*timeMultiplier >= colItemLeft &&
                           rightOfPlayer <= colItemRight &&
                           isVerticallyOverlaped;

                hitLeft = leftOfPlayer - velPlayer->x * delta * *(attritionCoefficient)*timeMultiplier <= colItemRight &&
                          leftOfPlayer >= colItemLeft &&
                          isVerticallyOverlaped;
            }

            if (hitRight) {
                playerInstance->colidedRight = true;
                velPlayer->x = -velPlayer->x * 0.2;
                posPlayer->x = colItemLeft - colisionBoxPlayer.x;
            }
            if (hitLeft) {
                playerInstance->colidedLeft = true;
                velPlayer->x = -velPlayer->x * 0.2;
                posPlayer->x = colItemRight;
            }
        }
    }
}

void Engine::HandleFPS(float loopBegin) {
    float timeStepInMS = 1000.0f / fpsCap;
    float loopStop = SDL_GetTicks();
    float timeDifference = timeStepInMS - (loopStop - loopBegin);
    SDL_Color fontColor = {0x00, 0xff, 0x00, 0x00};
    if (config->ShowFPSState() == true) {
        std::string fpsStr = "FPS: " +
                             std::to_string(int(fpsCap - (timeDifference / 1000.0f)));  // fps is in secods, timeDifference is in ms.
        DrawText(fpsStr, SDL_Rect{SCREEN_WIDTH - 100, 0, 100, 25}, fontColor);          // The 1000.0f is to transform the timeDiff to seconds
    }
    if (timeDifference >= 0) {
        SDL_Delay(timeStepInMS - (loopStop - loopBegin));
        return;
    }
}

void Engine::ShowDebugInfo() {
    SDL_Color fontColor = {0xff, 0xff, 0xff, 0xff};
    std::string levelItemStr = "LI: " + std::to_string(Level::colisions.size() + Level::textures.size());
    std::string colisionsAndTexturesStr = "C/T: " + std::to_string(Level::colisions.size()) + "/" + std::to_string(Level::textures.size());
    DrawText(levelItemStr, SDL_Rect{0, 5, GetTextRectangleWidth(levelItemStr.size()), 25}, fontColor);
    DrawText(colisionsAndTexturesStr, SDL_Rect{0, 30, GetTextRectangleWidth(colisionsAndTexturesStr.size()), 25}, fontColor);

    std::string playerInfo = "XY: " + std::to_string(playerInstance->pos.x) + " " + std::to_string(playerInstance->pos.y);
    DrawText(playerInfo, SDL_Rect{0, 55, GetTextRectangleWidth(playerInfo.size()), 25}, fontColor);
}

int Engine::GetTextRectangleWidth(size_t strSize) {
    return strSize * 15;
}

void Engine::HandleEvent(SDL_Event *event) {
    SDL_PollEvent(event);
    switch (event->type) {
        case SDL_QUIT: {
            quit = true;
            break;
        }
        case SDL_KEYDOWN: {
            keyStates[event->key.keysym.sym] = true;
            break;
        }
        case SDL_KEYUP: {
            keyStates[event->key.keysym.sym] = false;
            break;
        }
    }
    if (keyStates[SDLK_a]) {
        if (playerInstance->isPreparingToDash) {
            playerInstance->PrepareToDash(LEFT, 0, renderer, &timeMultiplier);
        } else {
            playerInstance->Move(MoveOptions::LEFT);
        }
    }
    if (keyStates[SDLK_d]) {
        if (playerInstance->isPreparingToDash) {
            playerInstance->PrepareToDash(RIGHT, 0, renderer, &timeMultiplier);
        } else {
            playerInstance->Move(MoveOptions::RIGHT);
        }
    }
    if (keyStates[SDLK_w]) {
        if (playerInstance->isPreparingToDash) {
            playerInstance->PrepareToDash(UP, 0, renderer, &timeMultiplier);
        } else {
            playerInstance->Move(MoveOptions::UP);
        }
    }
    if (keyStates[SDLK_s]) {
        if (playerInstance->isPreparingToDash) {
            playerInstance->PrepareToDash(DOWN, 0, renderer, &timeMultiplier);
        } else {
            playerInstance->Move(MoveOptions::DOWN);
        }
    }
    if (keyStates[SDLK_e]) {
        playerInstance->PrepareToDash(NONE, 0, renderer, &timeMultiplier);
        playerInstance->isPreparingToDash = true;
    }
    if (keyStates[SDLK_e] == false) {
        if (playerInstance->isPreparingToDash) {
            playerInstance->Dash();
            playerInstance->isPreparingToDash = false;
        }
    }
    if (keyStates[SDLK_LEFT]) {
        camera->Move(LEFT);
    }
    if (keyStates[SDLK_RIGHT]) {
        camera->Move(RIGHT);
    }
    if (keyStates[SDLK_UP]) {
        camera->Move(UP);
    }
    if (keyStates[SDLK_DOWN]) {
        camera->Move(DOWN);
    }

    if (keyStates[SDLK_r]) {
        playerInstance->pos = {0, 0};
    }
    if (keyStates[SDLK_q]) {
        quit = true;
        return;
    }
}

void Engine::Render() {
    Vec2f cameraPos = camera->pos;

    for (LevelItem levelItem : Level::colisions) {
        SDL_Color color = levelItem.color;
        SDL_Rect levelItemWireframe = {
            levelItem.wireframe.x - (int)cameraPos.x,
            levelItem.wireframe.y - (int)cameraPos.y,
            levelItem.wireframe.w,
            levelItem.wireframe.h,
        };
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &levelItemWireframe);
    }

    SDL_Rect playerModel = {
        (int)playerInstance->pos.x - (int)cameraPos.x,
        (int)playerInstance->pos.y - (int)cameraPos.y,
        playerInstance->hitbox.x,
        playerInstance->hitbox.y,
    };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
    SDL_RenderFillRect(renderer, &playerModel);
}

void Engine::DrawText(const std::string &text, SDL_Rect textureRect, const SDL_Color fontColor) {
    SDL_Surface *textSurface = TTF_RenderText_Blended(debugFont, text.c_str(), fontColor);
    if (!textSurface) {
        fprintf(stderr, "SDL TTF could not render text\n");
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        fprintf(stderr, "SDL TTF could not create texture\n");
        return;
    }

    SDL_RenderCopy(renderer, textTexture, NULL, &textureRect);

    SDL_FreeSurface(textSurface);
}

void Engine::Init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    printf("INFO: SDL_Init initialized succesfully\n");

    if (TTF_Init() != 0) {
        fprintf(stderr, "SDL TTF could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    printf("INFO: SDL_TTF initialized succesfully\n");

    window = SDL_CreateWindow("Game", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "SDL could not create window! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    printf("INFO: Window initialized succesfully\n");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "SDL could not create renderer! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    printf("INFO: Renderer initialized succesfully\n");

    if (debugMode) {
        printf("INFO: Starting in debug mode\n");
    }

    debugFont = TTF_OpenFont("./Include/Fonts/BigBlueTermMono.ttf", 15);
    if (!debugFont) {
        fprintf(stderr, "SDL TTF could not open font\n");
        exit(EXIT_FAILURE);
    }
    printf("INFO: Loaded Debug Font\n");
}

int Engine::Run() {
    printf("Starting the game loop...\n");
    Loop();

    printf("Game closed");
    SDL_DestroyWindow(window);
    TTF_CloseFont(debugFont);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

Engine *Engine::instance = new Engine;
Engine *Engine::GetEngineInstance() { return instance; }

Vec2i Engine::GetScreenInfo() { return {SCREEN_WIDTH, SCREEN_HEIGHT}; }

Vec2f Engine::GetCameraPos() { return camera->pos; }
