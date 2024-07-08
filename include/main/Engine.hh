#pragma once

#include <SDL2/SDL.h>

#include <functional>
#include <unordered_map>

#include "../assetHandling/Font.hh"
#include "Action.hh"

//------------------------------------------------------------------------------

class Engine {
   public:
    static Engine* GetEngineInstance();

    void Init();
    void Run();

   private:
    static Engine* instance;

    std::shared_ptr<Player> player = Player::Instance();
    ActionHandler* actionHandler = ActionHandler::Instance(&event, player.get(), &mousePos, &quit);

    SDL_Renderer* renderer = NULL;
    SDL_Window* window = NULL;
    SDL_Event event = SDL_Event{};

    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

    const Uint16 fpsMAX = 60;
    const Uint8 minFPS = 10;
    float timeDelta = 0;
    float timeMultiplier = 1;
    Uint32 lastLoopIteration = 0;

    Vec2<int> screenSpecs = {0, 0};
    Vec2<int> mousePos = {0, 0};

    bool quit = false;
    bool isPaused = false;

    Font debugFont = Font();

    static std::unordered_map<SDL_Keycode, std::function<void>> keymaps;

   private:
    void GameLoop();

    void UpdateTimeDelta();

    // If time multiplier gets changed, it will slowly make it go back to `1` for
    // a smooth increase in time
    void ResetTimeMultiplier();

    // Handles fps
    // @param float startTick | The first tick of the loop's iteration
    void HandleFPS(Uint32 startTick);

    // Updates screen Specifics to match real window size
    void UpdateScreenSpecs();

    // Shows debug info to the user // TODO:: needs to be improved
    void ShowDebugInfo();

    // Used for fonts // TODO:: NEEDS TO BE REDONE
    size_t GetTextRectangleWidth(size_t strSize);

    // Everything related to graphics in the main game loop
    void Render(Uint32 beginTick);

    void DrawMouse();
};

//------------------------------------------------------------------------------
