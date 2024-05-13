#include "../../include/main/Engine.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <sstream>

#include "../../include/main/Level.hh"
#include "../../lib/utils/sdl_utils.hh"

//------------------------------------------------------------------------------

void ClearBackground(SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b,
                     uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, NULL);
}

//------------------------------------------------------------------------------

void Engine::Loop() {
    float beginTick = 0;
    Vector2<float> *posPlayer = &playerInstance->pos;
    Vector2<float> *velPlayer = &playerInstance->velocity;
    Vector2<int> playerColisionboxInfo = playerInstance->GetHitboxInfo();
    // Level level;
    // level.GenerateLevel(0);

    while (!quit) {
        beginTick = SDL_GetTicks();
        HandlePlayerVelocity(posPlayer, velPlayer, playerColisionboxInfo);
        {  // Rendering
            ClearBackground(renderer, 100, 100, 100, 255);
            ShowDebugInfo();

            Render();
            HandleEvent(&event);
            HandleFPS(beginTick);
            config->DrawConfigMenu(window, renderer);

            SDL_RenderPresent(renderer);
            SDL_RenderClear(renderer);
        }
        // reset timeMultiplier
        if (timeMultiplier < 1 && !playerInstance->isPreparingToDash) {
            timeMultiplier += (1 - timeMultiplier) / 50;
        }
    }
}

void Engine::HandlePlayerVelocity(Vector2<float> *posPlayer,
                                  Vector2<float> *velPlayer,
                                  Vector2<int> playerColisionboxInfo) {
    Uint32 timeNow = SDL_GetTicks();

    float delta = (timeNow - lastUpdate) /
                  300.0f;  // 300 is just to make delta easier to work with

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

    HandlePlayerColisions(posPlayer, velPlayer, playerColisionboxInfo, delta,
                          &attritionCoefficient, timeMultiplier);

    velPlayer->x -=
        delta * attritionCoefficient * velPlayer->x * timeMultiplier;
    posPlayer->x += velPlayer->x * timeMultiplier;

    camera->FollowPlayer(*posPlayer, delta, {SCREEN_WIDTH, SCREEN_HEIGHT},
                         playerColisionboxInfo, timeMultiplier);

    lastUpdate = SDL_GetTicks();
}

void Engine::HandlePlayerColisions(Vector2<float> *posPlayer,
                                   Vector2<float> *velPlayer,
                                   const Vector2<int> colisionBoxPlayer,
                                   const float delta,
                                   float *attritionCoefficient,
                                   const float &timeMultipler) {
    (void)timeMultipler;  // just so the compilers doesnt bitch about it
    playerInstance->colidedUp = false;
    playerInstance->colidedLeft = false;
    playerInstance->colidedRight = false;
    playerInstance->colidedDown = false;

    playerInstance->isAbovePlatform = false;

    float headOfPlayer = posPlayer->y, leftOfPlayer = posPlayer->x,
          rightOfPlayer = posPlayer->x + colisionBoxPlayer.x,
          feetOfPLayer = posPlayer->y + colisionBoxPlayer.y;

    bool isHorizontallyOverlaped, hitFeet, hitHead, isVerticallyOverlaped,
        hitRight, hitLeft;

    float colItemTop, colItemLeft, colItemRight, colItemBottom;

    for (LevelItem colisionItem : Level::colisions) {
        {
            colItemTop = colisionItem.pos.y;
            colItemBottom = colisionItem.pos.y + colisionItem.wireframe.h;
            colItemLeft = colisionItem.pos.x;
            colItemRight = colisionItem.pos.x + colisionItem.wireframe.w;
        }

        {
            isHorizontallyOverlaped =
                (rightOfPlayer > colItemLeft && rightOfPlayer < colItemRight) ||
                (leftOfPlayer > colItemLeft && leftOfPlayer < colItemRight);

            hitFeet =
                feetOfPLayer + delta * velPlayer->y * timeMultiplier >=
                    colItemTop &&
                feetOfPLayer <=
                    colItemBottom - colisionItem.wireframe.h *
                                        0.8 &&  // 0.8 is the maximum that i've
                                                // found not to break colision,
                isHorizontallyOverlaped;        // this makes it so the player only
                                                // goes up if above 20% o the
                                                // colItem's height

            hitHead =
                headOfPlayer + delta * velPlayer->y * timeMultiplier <=
                    colItemBottom &&
                headOfPlayer >= colItemTop + colisionItem.wireframe.h * 0.9 &&
                isHorizontallyOverlaped;
        }

        if (hitFeet) {
            playerInstance->colidedDown = true;
            *attritionCoefficient = colisionItem.attritionCoefficient;
            posPlayer->y = colItemTop - colisionBoxPlayer.y;
            velPlayer->y = 0;
            if (colisionItem.colisionType == PLATFORM)
                playerInstance->isAbovePlatform = true;
        }

        if (colisionItem.colisionType != PLATFORM) {
            if (hitHead) {
                playerInstance->colidedUp = true;
                posPlayer->y = colItemBottom;
                velPlayer->y = -velPlayer->y * 0.2f;
            }
            {  // this needs to be recalculated after changing the players pos
               // in hitFeet and/or hitHead
                headOfPlayer = posPlayer->y, leftOfPlayer = posPlayer->x,
                rightOfPlayer = posPlayer->x + colisionBoxPlayer.x,
                feetOfPLayer = posPlayer->y + colisionBoxPlayer.y;

                isVerticallyOverlaped = ((headOfPlayer > colItemTop &&
                                          headOfPlayer < colItemBottom) ||
                                         (feetOfPLayer > colItemTop &&
                                          feetOfPLayer < colItemBottom));

                hitRight = rightOfPlayer -
                                   velPlayer->x * delta *
                                       *(attritionCoefficient)*timeMultiplier >=
                               colItemLeft &&
                           rightOfPlayer <= colItemRight &&
                           isVerticallyOverlaped;

                hitLeft =
                    leftOfPlayer - velPlayer->x * delta *
                                       *(attritionCoefficient)*timeMultiplier <=
                        colItemRight &&
                    leftOfPlayer >= colItemLeft && isVerticallyOverlaped;
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
    float loopEnd = SDL_GetTicks();
    float timeDifference = timeStepInMS - (loopEnd - loopBegin);

    if (config->ShowFPSState() == true) {
        std::stringstream fpsStr;  // fps is in secods, timeDifference is in ms.
        fpsStr << "FPS: " << std::setprecision(4)
               << fpsCap - (timeDifference /
                            1000.0f);  // that's why it's divided by 1000

        RenderTextSized(renderer, &debugFont, fpsStr.str().c_str(),
                        fpsStr.str().size(), Vector2<float>{0, 0}, GREEN, 3);
    }

    if (timeDifference >= 0) {
        SDL_Delay(timeStepInMS - (loopEnd - loopBegin));
        return;
    }
}

void Engine::ShowDebugInfo() {
    std::stringstream levelItemStr;
    levelItemStr << "LI: " << Level::colisions.size() + Level::textures.size()
                 << "C/T: " << Level::colisions.size() << "/"
                 << Level::textures.size();
    RenderTextSized(
        renderer, &debugFont, levelItemStr.str().c_str(),
        levelItemStr.str().size(),
        Vector2<float>{
            float(SCREEN_WIDTH -
                  GetTextRectangleWidth(levelItemStr.str().size()) * 2),
            0},
        WHITE, 3);

    std::stringstream playerInfo;
    playerInfo << "XY: " << std::setprecision(4) << playerInstance->pos.x << " "
               << std::setprecision(4) << playerInstance->pos.y;
    RenderTextSized(
        renderer, &debugFont, playerInfo.str().c_str(), playerInfo.str().size(),
        Vector2<float>{
            float(SCREEN_WIDTH -
                  GetTextRectangleWidth(playerInfo.str().size()) * 2),
            100},
        WHITE, 3);
}

int Engine::GetTextRectangleWidth(size_t strSize) { return strSize * 15; }

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
    if (keyStates[SDLK_e] &&
        SDL_GetTicks() >= playerInstance->whenNextDashAvailable) {
        if (!playerInstance->isPreparingToDash) {
            playerInstance->PrepareToDash(NONE, 0, renderer, &timeMultiplier);
            playerInstance->DashEnd = SDL_GetTicks() + 2000;
            playerInstance->isPreparingToDash = true;
        }
        if (SDL_GetTicks() >= playerInstance->DashEnd) {
            playerInstance->Dash();
            playerInstance->whenNextDashAvailable = SDL_GetTicks() + 5000;
        }
    }
    if (keyStates[SDLK_e] == false) {
        if (playerInstance->isPreparingToDash) {
            playerInstance->Dash();
            playerInstance->isPreparingToDash = false;
            playerInstance->whenNextDashAvailable = SDL_GetTicks() + 5000;
        }
    }
    if (keyStates[SDLK_o]) {
        config->ApplyConfig(window, renderer, Vector2<int *>{&SCREEN_WIDTH, &SCREEN_HEIGHT});
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
        playerInstance->velocity = {0, 0};
    }
    if (keyStates[SDLK_q]) {
        quit = true;
        return;
    }
}

void Engine::Render() {
    Vector2<float> cameraPos = camera->pos;

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

void Engine::Init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }
    printf("INFO: SDL_Init initialized succesfully\n");

    window = SDL_CreateWindow("Game",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_BORDERLESS);
    if (window == NULL) {
        fprintf(stderr, "SDL could not create window! SDL_Error: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }
    printf("INFO: Window initialized succesfully\n");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "SDL could not create renderer! SDL_Error: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }
    printf("INFO: Renderer initialized succesfully\n");

    debugFont = FontLoadFromFile(renderer,
                                 "./assets/fonts/charmap-oldschool_white.png");
    printf("INFO: Loaded Debug Font\n");

    {
        config->ApplyConfig(window, renderer, Vector2<int *>{&SCREEN_WIDTH, &SCREEN_HEIGHT});
        printf("INFO: Config read succesfully\n");
    }
}

int Engine::Run() {
    printf("Starting the game loop...\n");
    Loop();

    printf("Game closed");
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

Engine *Engine::instance = new Engine;
Engine *Engine::GetEngineInstance() { return instance; }

void Engine::UpdateScreenInfo() {
    SDL_GetRendererOutputSize(renderer, &SCREEN_WIDTH, &SCREEN_HEIGHT);
}
