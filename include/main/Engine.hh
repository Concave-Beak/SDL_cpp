#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <sys/wait.h>

#include <unordered_map>

#include "../assetHandeling/Font.hh"
#include "../entities/Camera.hh"
#include "../entities/Player.hh"
#include "./Config.hh"

//------------------------------------------------------------------------------

class Engine {
   public:
    bool debugMode = false;  // will change

    // SDL_Window* GetWindow();  // not implemented
    static Engine* GetEngineInstance();

    void UpdateScreenInfo();

    void Init();
    int Run();

    // float GetTimeMultiplier(); // not used
    Vec2f GetCameraPos();

   private:
    static Engine* instance;

    Player* playerInstance = Player::GetPlayerInstace();
    Config* config = Config::GetConfig();
    Camera* camera = Camera::GetCameraInstance();

    SDL_Renderer* renderer = NULL;
    SDL_Window* window = NULL;
    SDL_Event event = SDL_Event{};

    std::unordered_map<SDL_Keycode, bool> keyStates;

    const Uint16 fpsCap = 60;
    const Uint8 minFPS = 10;
    float timeMultiplier = 1;
    Uint32 lastUpdate = 0;

    bool quit = false;
    bool paused = false;

    int SCREEN_WIDTH = 1024;
    int SCREEN_HEIGHT = 768;

    Font debugFont;

   private:
    void Loop();

    void HandleFPS(float startTick);
    void HandleEvent(SDL_Event* event);

    void HandlePlayerVelocity(Vec2f* playerPos, Vec2f* playerVel, Vec2i playerHitboxInfo);
    void HandlePlayerColisions(Vec2f* playerPos, Vec2f* playerVel, Vec2i playerColisionboxInfo, float delta, float* attritionCoefficient, const float& timeMultiplier);

    void ShowDebugInfo();
    int GetTextRectangleWidth(size_t strSize);

    void Render();
};

//------------------------------------------------------------------------------
