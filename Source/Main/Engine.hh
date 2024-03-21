#ifndef __ENGINE_HEADER__
#define __ENGINE_HEADER__

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <sys/wait.h>
#include <string>
#include <unordered_map>

#include "../Entities/Player.hh"
#include "./Config.hh"

//------------------------------------------------------------------------------

class Engine {
   public:
    SDL_Window* GetWindow();  // not implemented
    static Engine* GetEngineInstance();
    Vec2i GetScreenInfo();
    void Init();
    int Run();

   private:
    static Engine* instance;

    Player* playerInstance = Player::GetPlayerInstace();
    Config* config = Config::GetConfig();

    SDL_Renderer* renderer = NULL;
    SDL_Window* window = NULL;
    SDL_Event event = SDL_Event{};

    std::unordered_map<SDL_Keycode, bool> keyStates;

    Uint16 fpsCap = 60;
    const Uint8 minFPS = 10;

    bool quit = false;
    bool paused = false;

    int SCREEN_WIDTH = 1024;
    int SCREEN_HEIGHT = 768;

   private:
    void Loop();

    void HandleFPS(float startTick);
    void HandleKeyboardEvents(SDL_Event* event);
    void HandlePlayerAccel(Vec2f* playerPos, Vec2f* playerVel, Vec2i playerHitboxInfo);

    void CheckColisions(Vec2f* playerPos, Vec2f* playerVel, Vec2i playerColisionboxInfo);

    void DrawText(const std::string& text);
};

//------------------------------------------------------------------------------

#endif
