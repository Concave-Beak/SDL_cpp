#pragma once

#include <SDL2/SDL.h>

#include "../../lib/utils/error_handling.hh"
#include "../assetHandling/Font.hh"
#include "../entities/Camera.hh"
#include "../entities/Player.hh"
#include "./Config.hh"

//------------------------------------------------------------------------------

class Engine {
   public:
    static Engine* GetEngineInstance();

    const Error Init();
    void Run();

   private:
    static Engine* instance;

    Player* playerInstance = Player::GetPlayerInstace();
    Config* configInstance = Config::GetConfig();
    Camera* cameraInstance = Camera::GetCameraInstance();

    SDL_Renderer* renderer = NULL;
    SDL_Window* window = NULL;
    SDL_Event event = SDL_Event{};

    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

    const Uint16 fpsMAX = 60;
    const Uint8 minFPS = 10;
    float timeDelta = 0;
    float timeMultiplier = 1;
    Uint32 lastLoopIteration = 0;

    Vector2<int> screenSpecs = {0, 0};

    bool quit = false;
    bool isPaused = false;

    Font debugFont;

   private:
    void GameLoop();

    void UpdateTimeDelta();

    // If time multiplier gets changed, it will slowly make it go back to `1` for
    // a smooth increase in time
    void ResetTimeMultiplier();

    // Handles fps
    // @param float startTick | The first tick of the loop's iteration
    void HandleFPS(float startTick);

    // Event Handling functions
    void HandleEvents();
    void HandleKeyboardState();
    void HandleMouseState();
    void HandleKeyboard(SDL_KeyboardEvent);
    void HandleMouse(SDL_MouseButtonEvent);

    // Updates screen Specifics to match real window size
    void UpdateScreenSpecs();

    // Shows debug info to the user // TODO:: needs to be improved
    void ShowDebugInfo();

    // Used for fonts // TODO:: NEEDS TO BE REDONE
    int GetTextRectangleWidth(size_t strSize);

    // Everything related to graphics in the main game loop
    void Render(float beginTick);
};

//------------------------------------------------------------------------------
