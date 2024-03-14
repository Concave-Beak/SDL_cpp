#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "Engine.hh"

#define GRAVITY 200.0f
#define MAX_X_SPEED 20
#define MIN_X_SPEED -20
#define MAX_Y_SPEED 400
#define MIN_Y_SPEED -200

//------------------------------------------------------------------------------

void ClearBackground(SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, NULL);
    SDL_RenderClear(renderer);
}

//------------------------------------------------------------------------------

void Engine::Loop() {
    float beginTick = 0;
    while (!quit) {
        beginTick = SDL_GetTicks();
        HandlePhysics();

        {  // Rendering
            ClearBackground(renderer, 100, 100, 100, 255);
            playerInstance->Draw(renderer);
            HandleFPS(beginTick);
            SDL_RenderPresent(renderer);
            SDL_RenderClear(renderer);
        }

        HandleKeyboardEvents(&event);
    }
}

void Engine::HandlePhysics() {
    Uint32 time = SDL_GetTicks();
    Vec2f *playerPos = &playerInstance->pos;
    Vec2f *playerVel = &playerInstance->velocity;
    Vec2i playerHitboxInfo = playerInstance->GetHitboxInfo();
    float dT = (time - playerInstance->lastUpdate) / 300.0f;

    if (playerPos->y < SCREEN_HEIGHT - playerHitboxInfo.y) {
        playerVel->y += dT * GRAVITY;
        playerPos->y += playerVel->y * dT;
    }

    if (playerVel->y > MAX_Y_SPEED) {
        playerVel->y = MAX_Y_SPEED;
    }

    if (playerVel->x > MAX_X_SPEED) {
        playerVel->x = MAX_X_SPEED;
    }
    if (playerVel->x < MIN_X_SPEED) {
        playerVel->x = MIN_X_SPEED;
    }
    playerPos->x += playerVel->x;
    playerVel->x *= 0.9;

    playerInstance->lastUpdate = time;
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

void Engine::HandleKeyboardEvents(SDL_Event *event) {
    SDL_PollEvent(event);
    switch (event->type) {
        case SDL_QUIT: {
            quit = true;
            break;
        }
        case SDL_KEYDOWN: {
            switch (event->key.keysym.sym) {
                case SDLK_LEFT: {
                    playerInstance->Move(MoveOpts::LEFT);
                    break;
                }
                case SDLK_RIGHT: {
                    playerInstance->Move(MoveOpts::RIGHT);
                    break;
                }
                case SDLK_UP: {
                    playerInstance->Move(MoveOpts::UP);
                    break;
                }
                case SDLK_DOWN: {
                    playerInstance->Move(MoveOpts::DOWN);
                    break;
                }
                case SDLK_q: {
                    quit = true;
                    break;
                }
                case SDLK_r: {
                    playerInstance->pos = {0, 0};
                    break;
                }
            }
        }
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
        fprintf(stderr,
                "SDL could not initialize! SDL_Error: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() != 0) {
        fprintf(stderr,
                "SDL TTF could not initialize! SDL_Error: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("Game",
                              0, 0,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr,
                "SDL could not create window! SDL_Error: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr,
                "SDL could not create renderer! SDL_Error: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

void Engine::Run() {
    printf("Starting the game loop...\n");
    Loop();

    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

Engine *Engine::instance = new Engine;
Engine *Engine::GetEngineInstance() { return instance; }

//------------------------------------------------------------------------------
/*
 *  TODO::
 *      - Improve fps handeling and physics
 *      - Create a pause tech
 */
