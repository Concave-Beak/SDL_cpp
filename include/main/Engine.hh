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
    // SDL_Window* GetWindow();  // not implemented
    static Engine* GetEngineInstance();

    const Error Init();
    void Run();

    // float GetTimeMultiplier(); // not used

   private:
    static Engine* instance;

    Player* playerInstance = Player::GetPlayerInstace();
    Config* config = Config::GetConfig();
    Camera* camera = Camera::GetCameraInstance();

    SDL_Renderer* renderer = NULL;
    SDL_Window* window = NULL;
    SDL_Event event = SDL_Event{};

    const Uint8* state = SDL_GetKeyboardState(NULL);

    const Uint16 fpsCap = 60;
    const Uint8 minFPS = 10;
    float delta = 0;
    float timeMultiplier = 1;
    Uint32 lastLoopIteration = 0;

    int SCREEN_WIDTH = 0;
    int SCREEN_HEIGHT = 0;

    bool quit = false;
    bool isPaused = false;

    Font debugFont;

   private:
    void GameLoop();

    void CalculateDelta();

    void ResetTimeMultiplier();

    void HandleFPS(float startTick);

    void HandleEvents();
    void HandleKeyboardState();
    void HandleMouseState();
    void HandleKeyboard(SDL_KeyboardEvent);
    void HandleMouse(SDL_MouseButtonEvent);

    void UpdateScreenSpecs();

    void ShowDebugInfo();
    int GetTextRectangleWidth(size_t strSize);

    void Render();
};

//------------------------------------------------------------------------------
