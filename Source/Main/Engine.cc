#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>

#include "Engine.hh"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define GRAVITY 200.0f;

//------------------------------------------------------------------------------

void ClearBackground(SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, NULL);
    SDL_RenderClear(renderer);
}

//------------------------------------------------------------------------------

void Engine::Loop() {
    float startTick = 0;
    while (!quit) {
        startTick = SDL_GetTicks();
        HandlePhysics();

        {  // Rendering
            ClearBackground(renderer, 100, 100, 100, 255);
            playerInstance->Draw(renderer);
            HandleFPS(startTick);
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
    float dT = (time - playerInstance->lastUpdate) / 300.0f;

    playerVel->y += dT * GRAVITY;
    playerPos->y += playerVel->y * dT;

    playerInstance->lastUpdate = time;
}

void Engine::HandleFPS(float startTick) {
    float timeStepInMS = 1000.0f / fpsCap;
    float lastLoopTick = SDL_GetTicks();
    if (timeStepInMS > (lastLoopTick - startTick)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(  // I saw that this is more precise
            int(timeStepInMS - (lastLoopTick - startTick))));   // than SDL_Delay()
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

Player *Engine::playerInstance = Player::GetPlayerInstace();

Engine *Engine::instance = new Engine;
Engine *Engine::GetEngineInstance() { return instance; }

//------------------------------------------------------------------------------
/*
 *  TODO::
 *      * Display fps case used wants to
 *      - Make fps calc faster and easier to understand
 *      - Improve fps handeling and physics
 *      - Create a pause tech
 */
