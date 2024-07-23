#include "../../include/main/Engine.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_video.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "../../include/assetHandling/UI/UI_Button.hh"
#include "../../include/game/entities/Camera.hh"
#include "../../include/game/entities/Creature/CreatureFactory.hh"
#include "../../include/main/Config.hh"
#include "../../include/main/Level.hh"
#include "../../lib/utils/debug_utils.hh"
#include "../../lib/utils/sdl_utils.hh"

//------------------------------------------------------------------------------

void ClearBackground(SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer);
}

//------------------------------------------------------------------------------

void Engine::GameLoop() {
    Uint32 beginTick = 0;
    new LevelItem(Vec2<int>{screenSpecs.x / 2, screenSpecs.y - 130}, {100, 30}, PLATFORM, SDL_Color{0, 0xff, 0, 0xff}, WOOD);                 // Placeholder
    new LevelItem(Vec2<int>{screenSpecs.x / 2, screenSpecs.y - 430}, {100, 100}, FULL_COLLISION, SDL_Color{0, 0xff, 0, 0xff}, STONE);         // Placeholder
    new LevelItem(Vec2<int>{screenSpecs.x / 4, screenSpecs.y - 185}, {100, 100}, FULL_COLLISION, SDL_Color{0, 0xff, 0, 0xff}, MUD);           // Placeholder
    new LevelItem(Vec2<int>{screenSpecs.x / 3 + 100, screenSpecs.y - 100}, {100, 100}, FULL_COLLISION, SDL_Color{0, 0xff, 0, 0xff}, MUD);     // Placeholder
    new LevelItem(Vec2<int>{-screenSpecs.x, screenSpecs.y - 5}, {screenSpecs.x * 3, 40}, FULL_COLLISION, SDL_Color{0, 0, 0xff, 0xff}, DIRT);  // Placeholder

    Creatures::CreatureFactory::Instance().CreateCreature(CreatureType::THIEF, {-1000, 0});
    while (!quit) {
        beginTick = SDL_GetTicks();
        ClearBackground(renderer, 100, 100, 100, 255);
        UpdateScreenSpecs();
        PlayerHandler::Instance().Handle(mousePos, Camera::Instance().GetCameraPos(), timeMultiplier, timeDelta, isPaused);
        Creatures::CreatureHandler::InvokeHandler(Camera::Instance().GetCameraPos(), renderer, timeDelta, timeMultiplier, isPaused);
        Attacks::AttackHandler::InvokeHandler(Camera::Instance().GetCameraPos(), renderer, timeDelta, timeMultiplier, isPaused);
        actionHandler->Handle();
        Render(beginTick);
        UpdateTimeDelta();
        ResetTimeMultiplier();
        lastLoopIteration = SDL_GetTicks();
    }
}

void Engine::ResetTimeMultiplier() {
    if (isPaused) return;

    if (timeMultiplier < 1) {
        timeMultiplier += (1 - timeMultiplier) / 50;
    }
}

void Engine::UpdateTimeDelta() {
    timeDelta = float(SDL_GetTicks() - lastLoopIteration) / 300.0f;  // 300 is just to make delta easier to work with
}

void Engine::HandleFPS(Uint32 loopBegin) {
    float timeStepInMS = 1000.0f / fpsMAX;
    Uint32 loopEnd = SDL_GetTicks();
    float timeDifference = timeStepInMS - float(loopEnd - loopBegin);

    if (Config::Instance().ShowFPSState() == true) {
        std::stringstream fpsStr;  // fps is in secods, timeDifference is in ms.
        fpsStr << "FPS: " << std::setprecision(4)
               << fpsMAX - (timeDifference / 1000.0f);  // that's why it's divided by 1000

        debugFont.RenderTextSized(renderer, fpsStr.str().c_str(), fpsStr.str().size(), Vec2<int>{0, 0}, SDL_Color{GREEN, 0xff}, 3);
    }

    if (timeDifference >= 0) {
        SDL_Delay(Uint32(timeStepInMS - float(loopEnd - loopBegin)));
        return;
    }
}

void Engine::ShowDebugInfo() {
    std::stringstream levelItemStr;
    levelItemStr << "LI: " << Level::collisions.size() + Level::textures.size()
                 << "C/T: " << Level::collisions.size() << "/" << Level::textures.size();
    debugFont.RenderTextSized(renderer, levelItemStr.str().c_str(), levelItemStr.str().size(), Vec2<int>{screenSpecs.x - int(GetTextRectangleWidth(levelItemStr.str().size()) * 2), 0}, SDL_Color{WHITE, 0xff}, 3);

    std::stringstream playerInfo;
    playerInfo << "XY: " << std::setprecision(4) << Player::GetPos().x << " " << std::setprecision(4)
               << Player::GetPos().y;
    debugFont.RenderTextSized(renderer, playerInfo.str().c_str(), playerInfo.str().size(), Vec2<int>{screenSpecs.x - int(GetTextRectangleWidth(playerInfo.str().size()) * 2), 100}, SDL_Color{WHITE, 0xff}, 3);
}

size_t Engine::GetTextRectangleWidth(size_t strSize) { return strSize * 15; }  // TODO

void Engine::Render(Uint32 beginTick) {
    Error err;
    Vec2<int> cameraPos = Camera::Instance().GetCameraPos();

    {
        Level::Draw(cameraPos, renderer);
        ShowDebugInfo();
        HandleFPS(beginTick);
        UI::Button::Handle(event, renderer).Handle();
        Camera::Instance().FollowPlayer(Player::GetPos(), timeDelta, screenSpecs,
                                        {Player::GetModel().w, Player::GetModel().h}, timeMultiplier, isPaused);
        DrawMouse();
        SDL_RenderPresent(renderer);
    }
}

void Engine::DrawMouse() {
    SDL_Rect attackModel = {
        mousePos.x - 10,
        mousePos.y - 10,
        20,
        20,
    };
    scc(SDL_SetRenderDrawColor(renderer, BLACK, 0xff)).Handle();
    scc(SDL_RenderFillRect(renderer, &attackModel)).Handle();
}

void Engine::Init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        exit(1);
    }
    PrintInfo(Info::SDL_INITIALIZED_SUCESSFULY, "");

    window = SDL_CreateWindow("SoulBound",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_CENTERED,
                              screenSpecs.x, screenSpecs.y,
                              SDL_WINDOW_BORDERLESS);
    if (window == NULL) {
        exit(1);
    }
    SDL_SetWindowGrab(window, SDL_TRUE);
    scc(SDL_SetRelativeMouseMode(SDL_TRUE)).Handle();
    PrintInfo(Info::SDL_WINDOW_INITIALIZED, "");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        exit(1);
    }
    scc(SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND)).Handle();
    PrintInfo(Info::SDL_RENDERER_INITIALIZED, "");

    debugFont = Font(renderer, "./assets/fonts/charmap-oldschool_white.png");
    if (debugFont.IsEmpty()) {
        exit(1);
    }
    PrintInfo(Info::FONT_LOADED, "debug font");

    Error err = Config::Instance().ApplyConfig(window, renderer, Vec2<int *>{&screenSpecs.x, &screenSpecs.y}, actionHandler);
    if (!err.IsEmpty()) {
        err.Handle();
    }
    PrintInfo(Info::CONFIG_READ_SUCESSFULLY, "");

    playerHandler = PlayerHandler::Init(renderer);  // remove Init
    if (playerHandler == nullptr) {
        exit(1);
    }

    srand(time(NULL));
}

void Engine::Run() {
    PrintInfo(Info::STARTING_GAME_LOOP, "");
    GameLoop();

    PrintInfo(Info::GAME_CLOSED, "");
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
}

Engine *Engine::instance = new Engine;
Engine *Engine::GetEngineInstance() { return instance; }

void Engine::UpdateScreenSpecs() { SDL_GetRendererOutputSize(renderer, &screenSpecs.x, &screenSpecs.y); }
