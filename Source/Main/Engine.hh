#ifndef __ENGINE_HEADER__
#define __ENGINE_HEADER__

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_ttf.h>
#include <sys/wait.h>

#include <string>
#include <unordered_map>

#include "../Entities/Player.hh"
#include "./Camera.hh"
#include "./Config.hh"

//------------------------------------------------------------------------------

class Engine {
   public:
    bool debugMode = false;  // will change

    // SDL_Window* GetWindow();  // not implemented
    static Engine* GetEngineInstance();

    Vec2i GetScreenInfo();

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

    Uint16 fpsCap = 60;
    const Uint8 minFPS = 10;
    float timeMultiplier = 1;

    bool quit = false;
    bool paused = false;

    int SCREEN_WIDTH = 1024;
    int SCREEN_HEIGHT = 768;

    TTF_Font* debugFont;

    Uint32 lastUpdate = 0;

   private:
    void Loop();

    void HandleFPS(float startTick);
    void HandleEvent(SDL_Event* event);

    void HandleVelocity(Vec2f* playerPos, Vec2f* playerVel, Vec2i playerHitboxInfo);
    void HandleColisions(Vec2f* playerPos, Vec2f* playerVel, Vec2i playerColisionboxInfo, float delta);

    void DrawText(const std::string& text, SDL_Rect textureRect, const SDL_Color fontColor);
    void ShowDebugInfo();
    int GetTextRectangleWidth(size_t strSize);

    void Render();
};

//------------------------------------------------------------------------------

#endif
