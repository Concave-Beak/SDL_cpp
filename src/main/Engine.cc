#include "../../include/main/Engine.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

#include <sstream>

#include "../../include/assetHandling/UI/UI_Button.hh"
#include "../../include/entities/NPC.hh"
#include "../../include/main/Level.hh"
#include "../../lib/utils/sdl_utils.hh"

//------------------------------------------------------------------------------

void ClearBackground(SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, NULL);
}

//------------------------------------------------------------------------------

void Engine::GameLoop() {
    Uint32 beginTick = 0;
    new LevelItem(Vector2<int>{screenSpecs.x / 2, screenSpecs.y - 130}, {100, 30}, PLATFORM, SDL_Color{0, 0xff, 0, 0xff}, WOOD);                // Placeholder
    new LevelItem(Vector2<int>{screenSpecs.x / 2, screenSpecs.y - 430}, {100, 100}, FULL_COLISION, SDL_Color{0, 0xff, 0, 0xff}, STONE);         // Placeholder
    new LevelItem(Vector2<int>{screenSpecs.x / 4, screenSpecs.y - 185}, {100, 100}, FULL_COLISION, SDL_Color{0, 0xff, 0, 0xff}, MUD);           // Placeholder
    new LevelItem(Vector2<int>{-screenSpecs.x, screenSpecs.y - 5}, {screenSpecs.x * 3, 40}, FULL_COLISION, SDL_Color{0, 0, 0xff, 0xff}, DIRT);  // Placeholder

    NPC npc(Entity::GENERIC_HUMANOID_ENEMY);
    while (!quit) {
        UpdateScreenSpecs();  // this should only be used after changing the resolution through config
        beginTick = SDL_GetTicks();
        Entity::Handle(timeDelta, timeMultiplier, isPaused);
        lastLoopIteration = SDL_GetTicks();

        NPC::HandleMovement(playerInstance->GetPos());  // Placeholder
        Render(beginTick);
        UpdateTimeDelta();
        ResetTimeMultiplier();
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

        RenderTextSized(renderer, &debugFont, fpsStr.str().c_str(), fpsStr.str().size(), Vector2<int>{0, 0}, SDL_Color{GREEN}, 3);
    }

    if (timeDifference >= 0) {
        SDL_Delay(Uint32(timeStepInMS - float(loopEnd - loopBegin)));
        return;
    }
}

void Engine::ShowDebugInfo() {
    std::stringstream levelItemStr;
    levelItemStr << "LI: " << Level::colisions.size() + Level::textures.size()
                 << "C/T: " << Level::colisions.size() << "/" << Level::textures.size();
    RenderTextSized(renderer, &debugFont, levelItemStr.str().c_str(), levelItemStr.str().size(), Vector2<int>{screenSpecs.x - int(GetTextRectangleWidth(levelItemStr.str().size()) * 2), 0}, SDL_Color{WHITE}, 3);

    std::stringstream playerInfo;
    playerInfo << "XY: " << std::setprecision(4) << playerInstance->GetPos().x << " " << std::setprecision(4)
               << playerInstance->GetPos().y;
    RenderTextSized(renderer, &debugFont, playerInfo.str().c_str(), playerInfo.str().size(), Vector2<int>{screenSpecs.x - int(GetTextRectangleWidth(playerInfo.str().size()) * 2), 100}, SDL_Color{WHITE}, 3);
}

size_t Engine::GetTextRectangleWidth(size_t strSize) { return strSize * 15; }  // TODO

void Engine::HandleEvents() {
    SDL_PollEvent(&event);
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
    HandleKeyboardState();
    HandleMouseState();
}

void Engine::HandleKeyboard(SDL_KeyboardEvent kbEvent) {
    // if (kbEvent.keysym.sym == SDLK_o) {
    //     config->ToggleMenuVisibility();
    // }
    if (kbEvent.keysym.sym == SDLK_ESCAPE) {
    }
}

void Engine::HandleMouse(SDL_MouseButtonEvent mbEvent) {
    (void)mbEvent;
}

void Engine::HandleKeyboardState() {
    if (keyboardState[SDL_SCANCODE_A]) {
        playerInstance->entity.Move(Direction::LEFT, isPaused);
    }
    if (keyboardState[SDL_SCANCODE_D]) {
        playerInstance->entity.Move(Direction::RIGHT, isPaused);
    }
    if (keyboardState[SDL_SCANCODE_W]) {
        playerInstance->entity.Move(Direction::UP, isPaused);
    }
    if (keyboardState[SDL_SCANCODE_S]) {
        playerInstance->entity.Move(Direction::DOWN, isPaused);
    }
    if (keyboardState[SDL_SCANCODE_LEFT]) {
        cameraInstance->Move(LEFT, isPaused);
    }
    if (keyboardState[SDL_SCANCODE_RIGHT]) {
        cameraInstance->Move(RIGHT, isPaused);
    }
    if (keyboardState[SDL_SCANCODE_UP]) {
        cameraInstance->Move(UP, isPaused);
    }
    if (keyboardState[SDL_SCANCODE_DOWN]) {
        cameraInstance->Move(DOWN, isPaused);
    }

    if (keyboardState[SDL_SCANCODE_R]) {
        // playerInstance->pos = {0, 0};
        // playerInstance->velocity = {0, 0};
    }
    if (keyboardState[SDL_SCANCODE_Q]) {
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

void Engine::Render(Uint32 beginTick) {
    Vector2<int> cameraPos = {(int)cameraInstance->pos.x, (int)cameraInstance->pos.y};
    ClearBackground(renderer, 100, 100, 100, 255);

    {
        Level::Draw(cameraPos, renderer);
        Entity::Draw(cameraPos, renderer);
        ShowDebugInfo();
        HandleEvents();
        HandleFPS(beginTick);
        UI::Button::Handle(event, renderer);
        cameraInstance->FollowPlayer(playerInstance->GetPos(), timeDelta, screenSpecs,
                                     playerInstance->GetHitbox(), timeMultiplier, isPaused);

        SDL_RenderPresent(renderer);
        scc(SDL_RenderClear(renderer));
    }
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
        configInstance->ApplyConfig(window, renderer, Vector2<int *>{&screenSpecs.x, &screenSpecs.y});
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

void Engine::UpdateScreenSpecs() { SDL_GetRendererOutputSize(renderer, &screenSpecs.x, &screenSpecs.y); }
