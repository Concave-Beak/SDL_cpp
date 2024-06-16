#include "../../include/main/Engine.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_video.h>

#include <iostream>
#include <sstream>

#include "../..//include/game/entities/Attack.hh"
#include "../..//include/game/entities/NPC.hh"
#include "../../include/assetHandling/UI/UI_Button.hh"
#include "../../include/main/Level.hh"
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

    NPC npc(Entity::GENERIC_HUMANOID_ENEMY, {1200, 900});
    while (!quit) {
        beginTick = SDL_GetTicks();
        ClearBackground(renderer, 100, 100, 100, 255);
        UpdateScreenSpecs();
        Entity::Handle(timeDelta, timeMultiplier, isPaused, cameraInstance->GetCameraPos(), renderer);
        Player::Handle(mousePos);
        actionHandler->Handle();
        NPC::Handle(renderer, playerInstance->GetPos(), cameraInstance->GetCameraPos(), playerInstance->GetHitbox());  // Placeholder
        Attack::CheckAndDestroyExpiredAttacks();
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

    if (configInstance->ShowFPSState() == true) {
        std::stringstream fpsStr;  // fps is in secods, timeDifference is in ms.
        fpsStr << "FPS: " << std::setprecision(4)
               << fpsMAX - (timeDifference / 1000.0f);  // that's why it's divided by 1000

        RenderTextSized(renderer, &debugFont, fpsStr.str().c_str(), fpsStr.str().size(), Vec2<int>{0, 0}, SDL_Color{GREEN, 0xff}, 3);
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
    RenderTextSized(renderer, &debugFont, levelItemStr.str().c_str(), levelItemStr.str().size(), Vec2<int>{screenSpecs.x - int(GetTextRectangleWidth(levelItemStr.str().size()) * 2), 0}, SDL_Color{WHITE, 0xff}, 3);

    std::stringstream playerInfo;
    playerInfo << "XY: " << std::setprecision(4) << playerInstance->GetPos().x << " " << std::setprecision(4)
               << playerInstance->GetPos().y;
    RenderTextSized(renderer, &debugFont, playerInfo.str().c_str(), playerInfo.str().size(), Vec2<int>{screenSpecs.x - int(GetTextRectangleWidth(playerInfo.str().size()) * 2), 100}, SDL_Color{WHITE, 0xff}, 3);
}

size_t Engine::GetTextRectangleWidth(size_t strSize) { return strSize * 15; }  // TODO

void Engine::Render(Uint32 beginTick) {
    Vec2<int> cameraPos = cameraInstance->GetCameraPos();

    {
        Level::Draw(cameraPos, renderer);
        ShowDebugInfo();
        // HandleEvents();
        HandleFPS(beginTick);
        UI::Button::Handle(event, renderer);
        cameraInstance->FollowPlayer(playerInstance->GetPos(), timeDelta, screenSpecs,
                                     playerInstance->GetHitbox(), timeMultiplier, isPaused);
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
    scc(SDL_SetRenderDrawColor(renderer, BLACK, 0xff));
    scc(SDL_RenderFillRect(renderer, &attackModel));
}

const Error Engine::Init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Crash(Error(ErrorCode::SDL_FUNCTION_ERROR, "Couldn't init SDL", Severity::HIGH));
    }
    std::cout << "INFO: SDL_Init initialized succesfully\n";

    window = SDL_CreateWindow("SoulBound", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_CENTERED, screenSpecs.x, screenSpecs.y, SDL_WINDOW_BORDERLESS);
    if (window == NULL) {
        Crash(Error(ErrorCode::SDL_FUNCTION_ERROR, "Couldn't init SDL", Severity::HIGH));
    }
    std::cout << "INFO: Window initialized succesfully\n";

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        Crash(Error(ErrorCode::SDL_FUNCTION_ERROR, "Couldn't init SDL", Severity::HIGH));
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    std::cout << "INFO: Renderer initialized succesfully\n";

    debugFont = FontLoadFromFile(renderer, "./assets/fonts/charmap-oldschool_white.png");
    std::cout << "INFO: Loaded Debug Font\n";

    {
        configInstance->ApplyConfig(window, renderer, Vec2<int *>{&screenSpecs.x, &screenSpecs.y}, actionHandler);
        std::cout << "INFO: Config read succesfully\n";
    }

    SDL_SetWindowGrab(window, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

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

void Engine::UpdateScreenSpecs() { SDL_GetRendererOutputSize(renderer, &screenSpecs.x, &screenSpecs.y); }
