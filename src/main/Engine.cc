#include "../../include/main/Engine.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <sstream>

#include "../../include/assetHandling/UI/UI_Button.hh"
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
    UI::Button btn(UI::Button::ButtonFlags::TEXTURE_BUTTON,
                   SDL_Rect{.x = 100, .y = 100, .w = 64, .h = 64},
                   "myBtn");
    btn.SetTexture(renderer, UI::Button::DEFAULT_TEXTURE, "./assets/menu/unchecked-button.png");  // placeholder
    btn.SetTexture(renderer, UI::Button::CLICKED_TEXTURE, "./assets/menu/checked-button.png");
    btn.SetFlags(UI::Button::SWITCH_BUTTON | UI::Button::TEXTURE_BUTTON);

    UI::Button btn1(UI::Button::ButtonFlags::TEXTURE_BUTTON,
                    SDL_Rect{.x = 400, .y = 100, .w = 64, .h = 64},
                    "myBtn");
    btn1.SetTexture(renderer, UI::Button::DEFAULT_TEXTURE, "./assets/menu/unchecked-button.png");  // placeholder
    btn1.SetTexture(renderer, UI::Button::CLICKED_TEXTURE, "./assets/menu/checked-button.png");
    btn1.SetFlags(UI::Button::SWITCH_BUTTON | UI::Button::TEXTURE_BUTTON);

    float beginTick = 0;
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
            HandleEvents();
            HandleFPS(beginTick);
            config->DrawConfigMenu(window, renderer);
            camera->FollowPlayer(playerInstance->GetPos(), delta, {SCREEN_WIDTH, SCREEN_HEIGHT},
                                 playerColisionboxInfo, timeMultiplier);

            SDL_RenderPresent(renderer);
            scc(SDL_RenderClear(renderer));

            CalculateDelta();
        }
        ResetTimeMultiplier();
    }
}

void Engine::ResetTimeMultiplier() {
    if (timeMultiplier < 1 && !playerInstance->isPreparingToDash) {
        timeMultiplier += (1 - timeMultiplier) / 50;
    }
}

void Engine::CalculateDelta() { delta = (SDL_GetTicks() - lastLoopIteration) / 300.0f; }  // 300 is just to make delta easier to work with

void Engine::HandleFPS(float loopBegin) {
    float timeStepInMS = 1000.0f / fpsCap;
    float loopEnd = SDL_GetTicks();
    float timeDifference = timeStepInMS - (loopEnd - loopBegin);

    if (config->ShowFPSState() == true) {
        std::stringstream fpsStr;                                                          // fps is in secods, timeDifference is in ms.
        fpsStr << "FPS: " << std::setprecision(4) << fpsCap - (timeDifference / 1000.0f);  // that's why it's divided by 1000

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
    playerInfo << "XY: " << std::setprecision(4) << playerInstance->GetPos().x << " "
               << std::setprecision(4) << playerInstance->GetPos().y;
    RenderTextSized(renderer, &debugFont, playerInfo.str().c_str(), playerInfo.str().size(), Vector2<float>{float(SCREEN_WIDTH - GetTextRectangleWidth(playerInfo.str().size()) * 2), 100}, WHITE, 3);
}

int Engine::GetTextRectangleWidth(size_t strSize) { return strSize * 15; }  // TODO

void Engine::HandleEvents() {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                quit = true;
                break;
            }
            case SDL_KEYDOWN: {
                HandleKeyboard(event.key);
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                HandleMouse(event.button);
                break;
            }
        }
    }
    HandleKeyboardState();
    HandleMouseState();
}

void Engine::HandleKeyboard(SDL_KeyboardEvent kbEvent) {
}

void Engine::HandleMouse(SDL_MouseButtonEvent mbEvent) {
    UI::Button::HandleButtonClicks(Vector2<int>{.x = mbEvent.x, .y = mbEvent.y});
}

void Engine::HandleKeyboardState() {
    if (state[SDL_SCANCODE_A]) {
        if (playerInstance->isPreparingToDash) {
            playerInstance->PrepareToDash(LEFT, 0, renderer, &timeMultiplier);
        } else {
            playerInstance->Move(MoveOptions::LEFT);
        }
    }
    if (state[SDL_SCANCODE_D]) {
        if (playerInstance->isPreparingToDash) {
            playerInstance->PrepareToDash(RIGHT, 0, renderer, &timeMultiplier);
        } else {
            playerInstance->Move(MoveOptions::RIGHT);
        }
    }
    if (state[SDL_SCANCODE_W]) {
        if (playerInstance->isPreparingToDash) {
            playerInstance->PrepareToDash(UP, 0, renderer, &timeMultiplier);
        } else {
            playerInstance->Move(MoveOptions::UP);
        }
    }
    if (state[SDL_SCANCODE_S]) {
        if (playerInstance->isPreparingToDash) {
            playerInstance->PrepareToDash(DOWN, 0, renderer, &timeMultiplier);
        } else {
            playerInstance->Move(MoveOptions::DOWN);
        }
    }
    if (state[SDL_SCANCODE_E] && SDL_GetTicks() >= playerInstance->whenNextDashAvailable) {
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
    if (!state[SDL_SCANCODE_E]) {
        if (playerInstance->isPreparingToDash) {
            playerInstance->Dash();
            playerInstance->isPreparingToDash = false;
            playerInstance->whenNextDashAvailable = SDL_GetTicks() + 5000;
        }
    }
    if (state[SDL_SCANCODE_O]) {
        config->ApplyConfig(window, renderer, Vector2<int *>{&SCREEN_WIDTH, &SCREEN_HEIGHT});
    }
    if (state[SDL_SCANCODE_LEFT]) {
        camera->Move(LEFT);
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        camera->Move(RIGHT);
    }
    if (state[SDL_SCANCODE_UP]) {
        camera->Move(UP);
    }
    if (state[SDL_SCANCODE_DOWN]) {
        camera->Move(DOWN);
    }

    if (state[SDL_SCANCODE_R]) {
        // playerInstance->pos = {0, 0};
        // playerInstance->velocity = {0, 0};
    }
    if (state[SDL_SCANCODE_Q]) {
        quit = true;
        return;
    }
}

void Engine::HandleMouseState() {
    int x, y;
    Uint32 buttons = SDL_GetMouseState(&x, &y);

    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) {
    }
    if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
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
        (int)playerInstance->GetPos().x - (int)cameraPos.x,
        (int)playerInstance->GetPos().y - (int)cameraPos.y,
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
