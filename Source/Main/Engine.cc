#include "Engine.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "../../Include/Headers/Utils.hh"
#include "Level.hh"

//------------------------------------------------------------------------------

void ClearBackground(SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, NULL);
}

//------------------------------------------------------------------------------

void Engine::Loop() {
    float beginTick = 0;
    Vec2f *playerPos = &playerInstance->pos;
    Vec2f *playerVel = &playerInstance->velocity;
    Vec2i playerColisionboxInfo = playerInstance->GetHitboxInfo();

    new LevelItem(Vec2i{SCREEN_WIDTH / 2, SCREEN_HEIGHT - 130}, {100, 30}, PLATFORM, SDL_Color{0, 0xff, 0, 0xff});                                  // Placeolder
    new LevelItem(Vec2i{SCREEN_WIDTH / 2, SCREEN_HEIGHT - 430}, {100, 100}, FULL_COLISION, SDL_Color{0, 0xff, 0, 0xff});                            // Placeolder
    new LevelItem(Vec2i{SCREEN_WIDTH / 4, SCREEN_HEIGHT - playerColisionboxInfo.y - 110}, {100, 100}, FULL_COLISION, SDL_Color{0, 0xff, 0, 0xff});  // Placeolder
    new LevelItem(Vec2i{0, SCREEN_HEIGHT - 5}, {SCREEN_WIDTH, 40}, FULL_COLISION, SDL_Color{0, 0, 0xff, 0xff});                                     // Placeolder

    while (!quit) {
        beginTick = SDL_GetTicks();
        HandlePlayerVel(playerPos, playerVel, playerColisionboxInfo);

        {  // Rendering
            ClearBackground(renderer, 100, 100, 100, 255);
            Level::GenerateLevel(0, renderer);
            playerInstance->Draw(renderer);
            HandleFPS(beginTick);
            SDL_RenderPresent(renderer);
            SDL_RenderClear(renderer);
        }

        HandleEvent(&event);
    }
}

void Engine::HandlePlayerVel(Vec2f *posPlayer, Vec2f *velPlayer, Vec2i playerColisionboxInfo) {
    Uint32 timeNow = SDL_GetTicks();
    float delta = (timeNow - playerInstance->lastUpdate) / 300.0f;  // 300 is just to make delta easier to handle

    HandleColisions(posPlayer, velPlayer, playerColisionboxInfo, delta);

    velPlayer->x *= 1.0f - delta;
    posPlayer->x += velPlayer->x;

    if (playerInstance->colidedDown == false) {
        velPlayer->y += delta * GRAVITY;
        posPlayer->y += velPlayer->y * delta;
    }

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

    playerInstance->lastUpdate = timeNow;
}

void Engine::HandleColisions(Vec2f *posPlayer, Vec2f *velPlayer, Vec2i colisionBoxPlayer, float delta) {
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

    int colItemTop,
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

            hitFeet = feetOfPLayer + delta * velPlayer->y >= colItemTop &&
                      feetOfPLayer <= colItemBottom - colisionItem.wireframe.h * 0.9 &&  // 0.9 is the maximum that i've found not to break colision
                      isHorizontallyOverlaped;

            hitHead = headOfPlayer + delta * velPlayer->y <= colItemBottom &&
                      headOfPlayer >= colItemTop + colisionItem.wireframe.h * 0.9 &&
                      isHorizontallyOverlaped &&
                      colisionItem.colisionType != PLATFORM;

            isVerticallyOverlaped = ((headOfPlayer > colItemTop && headOfPlayer < colItemBottom) ||
                                     (feetOfPLayer > colItemTop && feetOfPLayer < colItemBottom));

            hitRight = rightOfPlayer + velPlayer->x * (1 - delta) >= colItemLeft &&
                       rightOfPlayer <= colItemRight &&
                       isVerticallyOverlaped;

            hitLeft = leftOfPlayer + velPlayer->x * (1 - delta) <= colItemRight &&
                      leftOfPlayer >= colItemLeft &&
                      isVerticallyOverlaped;
        }

        if (hitFeet) {
            playerInstance->colidedDown = true;
            posPlayer->y = colItemTop - colisionBoxPlayer.y;
            velPlayer->y = 0;
            if (colisionItem.colisionType == PLATFORM) playerInstance->isAbovePlatform = true;
        }

        if (colisionItem.colisionType != PLATFORM) {
            if (hitHead) {
                playerInstance->colidedUp = true;
                posPlayer->y = colItemBottom;
                velPlayer->y = -velPlayer->y * 0.1;
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
    if (config->ShowFPSState()) {
        std::string fpsStr = "FPS: " +
                             std::to_string(int(fpsCap - (timeDifference / 1000.0f)));  // fps is in secods, timeDifference is in ms.
        DrawText(fpsStr);                                                               // The 1000.0f is to transform the timeDiff to seconds
    }
    if (timeDifference >= 0) {
        SDL_Delay(timeStepInMS - (loopStop - loopBegin));
        return;
    }
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
    if (keyStates[SDLK_LEFT] || keyStates[SDLK_a]) {
        playerInstance->Move(MoveOpts::LEFT);
    }
    if (keyStates[SDLK_RIGHT] || keyStates[SDLK_d]) {
        playerInstance->Move(MoveOpts::RIGHT);
    }
    if (keyStates[SDLK_UP] || keyStates[SDLK_w]) {
        playerInstance->Move(MoveOpts::UP);
    }
    if (keyStates[SDLK_DOWN] || keyStates[SDLK_s]) {
        playerInstance->Move(MoveOpts::DOWN);
    }
    if (keyStates[SDLK_r]) {
        playerInstance->pos = {playerInstance->pos.x, 0};
    }
    if (keyStates[SDLK_q]) {
        quit = true;
        return;
    }
}

void Engine::DrawText(const std::string &text) {
    // TODO: make font an engine field
    TTF_Font *font = TTF_OpenFont("./Include/Fonts/BigBlueTermMono.ttf", 30);
    SDL_Color fontColor = {0xff, 0xff, 0xff, 0xff};

    if (!font) {
        fprintf(stderr, "SDL TTF could not open font");
        return;
    }

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text.c_str(), fontColor);
    if (!textSurface) {
        fprintf(stderr, "SDL TTF could not render text");
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        fprintf(stderr, "SDL TTF could not create texture");
        return;
    }

    SDL_Rect textureRect = {
        .x = 0,
        .y = 0,
        .w = 200,
        .h = 50,
    };
    SDL_RenderCopy(renderer, textTexture, NULL, &textureRect);

    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
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
    printf("INFO: SDl_TTF initialized succesfully\n");

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
}

int Engine::Run() {
    printf("Starting the game loop...\n");
    Loop();

    printf("Game closed");
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

Engine *Engine::instance = new Engine;
Engine *Engine::GetEngineInstance() { return instance; }

//------------------------------------------------------------------------------
/*
 *  TODO:
 *      - Create colisions and platforms
 *      - Create a pause tech
 *      - Fix issue: More fps = player moves faster
 */
