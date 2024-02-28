#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "Engine.hh"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define GRAVITY 200.0f;

//------------------------------------------------------------------------------

void ClearBackground(SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer);
}

//------------------------------------------------------------------------------

void Engine::Render() {
    ClearBackground(renderer, 0, 0, 0, 255);
    playerInstance->Draw(renderer);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);  // in case the renderer doesn't get cleaned
}

void Engine::HandlePhysics() {
    // TODO: make it smoother
    Uint32 time = SDL_GetTicks();
    Vec2f *playerPos = &playerInstance->pos;
    Vec2f *playerVel = &playerInstance->velocity;
    float dT = (time - playerInstance->lastUpdate) / 300.0f;

    playerVel->y += dT * GRAVITY;
    playerPos->x += playerVel->x * dT;
    playerPos->y += playerVel->y * dT;

    playerInstance->lastUpdate = time;
}

void Engine::Loop() {
    // Uint64 totalFrameTicks = 0; // not implemented yet
    // Uint64 totalFrames = 0;
    while (!quit) {
        // ++totalFrames;
        Uint64 startTicks = SDL_GetTicks();
        Uint64 startPerformance = SDL_GetPerformanceCounter();

        HandlePhysics();
        HandleKeyboardEvents(&event);

        Render();
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

void Engine::Run() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr,
                "SDL could not initialize! SDL_Error: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Window *window = SDL_CreateWindow("Game",
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

    {
        Loop();
        printf("Starting the game loop...\n");
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

Player *Engine::playerInstance = Player::GetPlayerInstace();

Engine *Engine::instance = new Engine;
Engine *Engine::GetEngineInstance() { return instance; }

//------------------------------------------------------------------------------
