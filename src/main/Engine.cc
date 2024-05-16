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

void Engine::GameLoop() {
    float beginTick = 0;
    Vector2<float> *posPlayer = &playerInstance->pos;
    Vector2<float> *velPlayer = &playerInstance->velocity;
    Vector2<int> playerColisionboxInfo = playerInstance->GetHitboxInfo();
    new LevelItem(Vector2<int>{SCREEN_WIDTH / 2, SCREEN_HEIGHT - 130}, {100, 30}, PLATFORM, SDL_Color{0, 0xff, 0, 0xff}, WOOD);                                 // Placeholder
    new LevelItem(Vector2<int>{SCREEN_WIDTH / 2, SCREEN_HEIGHT - 430}, {100, 100}, FULL_COLISION, SDL_Color{0, 0xff, 0, 0xff}, STONE);                          // Placeholder
    new LevelItem(Vector2<int>{SCREEN_WIDTH / 4, SCREEN_HEIGHT - playerColisionboxInfo.y - 110}, {100, 100}, FULL_COLISION, SDL_Color{0, 0xff, 0, 0xff}, MUD);  // Placeholder
    new LevelItem(Vector2<int>{0, SCREEN_HEIGHT - 5}, {SCREEN_WIDTH, 40}, FULL_COLISION, SDL_Color{0, 0, 0xff, 0xff}, DIRT);                                    // Placeholder

    while (!quit) {
        beginTick = SDL_GetTicks();
        playerInstance->HandleColisions(delta, timeMultiplier);
        playerInstance->HandleVelocity(delta, timeMultiplier);
        lastLoopIteration = SDL_GetTicks();
        {  // Rendering
            ClearBackground(renderer, 100, 100, 100, 255);
            ShowDebugInfo();

            Render();
            HandleEvent(&event);
            HandleFPS(beginTick);
            config->DrawConfigMenu(window, renderer);
            camera->FollowPlayer(*posPlayer, delta, {SCREEN_WIDTH, SCREEN_HEIGHT},
                                 playerColisionboxInfo, timeMultiplier);

            SDL_RenderPresent(renderer);
            scc(SDL_RenderClear(renderer));

            CalculateDelta();
        }
        // reset timeMultiplier
        if (timeMultiplier < 1 && !playerInstance->isPreparingToDash) {
            timeMultiplier += (1 - timeMultiplier) / 50;
        }
    }
}

void Engine::CalculateDelta() { delta = (SDL_GetTicks() - lastLoopIteration) / 300.0f /*300 is just to make delta easier to work with*/; }

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
    RenderTextSized(renderer, &debugFont, levelItemStr.str().c_str(), levelItemStr.str().size(), Vector2<float>{float(SCREEN_WIDTH - GetTextRectangleWidth(levelItemStr.str().size()) * 2), 0}, WHITE, 3);

    std::stringstream playerInfo;
    playerInfo << "XY: " << std::setprecision(4) << playerInstance->pos.x << " "
               << std::setprecision(4) << playerInstance->pos.y;
    RenderTextSized(renderer, &debugFont, playerInfo.str().c_str(), playerInfo.str().size(), Vector2<float>{float(SCREEN_WIDTH - GetTextRectangleWidth(playerInfo.str().size()) * 2), 100}, WHITE, 3);
}

int Engine::GetTextRectangleWidth(size_t strSize) { return strSize * 15; }  // TODO

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
        scc(SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a));
        scc(SDL_RenderFillRect(renderer, &levelItemWireframe));
    }

    SDL_Rect playerModel = {
        (int)playerInstance->pos.x - (int)cameraPos.x,
        (int)playerInstance->pos.y - (int)cameraPos.y,
        playerInstance->hitbox.x,
        playerInstance->hitbox.y,
    };
    scc(SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0));
    scc(SDL_RenderFillRect(renderer, &playerModel));
}

const Error Engine::Init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Crash(Error(ErrorCode::SDL_FUNCTION_ERROR, "Couldn't init SDL", Severity::HIGH));
    }
    std::cout << "INFO: SDL_Init initialized succesfully\n";

    window = SDL_CreateWindow("SoulBound",
                              0, 0,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_BORDERLESS);
    if (window == NULL) {
        Crash(Error(ErrorCode::SDL_FUNCTION_ERROR, "Couldn't init SDL", Severity::HIGH));
    }
    std::cout << "INFO: Window initialized succesfully\n";

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        Crash(Error(ErrorCode::SDL_FUNCTION_ERROR, "Couldn't init SDL", Severity::HIGH));
    }
    std::cout << "INFO: Renderer initialized succesfully\n";

    debugFont = FontLoadFromFile(renderer, "./assets/fonts/charmap-oldschool_white.png");
    std::cout << "INFO: Loaded Debug Font\n";

    {
        config->ApplyConfig(window, renderer, Vector2<int *>{&SCREEN_WIDTH, &SCREEN_HEIGHT});
        std::cout << "INFO: Config read succesfully\n";
    }
    return Error{};
}

void Engine::Run() {
    std::cout << "Starting the game loop...\n";
    GameLoop();

    std::cout << "Game closed\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
}

Engine *Engine::instance = new Engine;
Engine *Engine::GetEngineInstance() { return instance; }

void Engine::UpdateScreenInfo() {
    SDL_GetRendererOutputSize(renderer, &SCREEN_WIDTH, &SCREEN_HEIGHT);
}
