#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "Engine.hh"

void ClearBackground(SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, NULL);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}

Engine *Engine::instance = new Engine;
Engine *Engine::GetEngineInstance() {
    return instance;
}

Player *Engine::player_instance = Player::GetPlayerInstace();

void Engine::HandleKeyboardEvents(SDL_Event *event) {
    SDL_PollEvent(event);
    switch (event->type) {
        case SDL_KEYDOWN: {
            switch (event->key.keysym.sym) {
                case SDLK_LEFT: {
                    player_instance->Move(MoveOpts::LEFT);
                    break;
                }
                case SDLK_RIGHT: {
                    player_instance->Move(MoveOpts::RIGHT);
                    break;
                }
                case SDLK_UP: {
                    player_instance->Move(MoveOpts::UP);
                    break;
                }
                case SDLK_DOWN: {
                    player_instance->Move(MoveOpts::DOWN);
                    break;
                }
            }
        }
    }
    if (event->type == SDL_QUIT) {
        quit = true;
    }
}

void Engine::Init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr,
                "SDL could not initialize! SDL_Error: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Window *window = SDL_CreateWindow("My window",
                                          0, 0,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr,
                "SDL could not create window! SDL_Error: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }

#if 0
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (surface == NULL) {
        fprintf(stderr,
                "SDL could not create surface! SDL_Error: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_UpdateWindowSurface(window);
#endif

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL) {
        fprintf(stderr,
                "SDL could not create renderer! SDL_Error: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_Event event;

    while (!quit) {
        ClearBackground(renderer, 0, 0, 0, 255);
        player_instance->Draw(renderer);
        HandleKeyboardEvents(&event);

        SDL_Delay(10);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
}
