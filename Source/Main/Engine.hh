#ifndef __ENGINE_HEADER__
#define __ENGINE_HEADER__

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <string>

#include "../Entities/Player.hh"

//------------------------------------------------------------------------------

class Engine {
   public:
   public:
    SDL_Window* GetWindow();  // not implemented
    static Engine* GetEngineInstance();
    void Init();
    void Run();

   private:
    static Player* playerInstance;
    static Engine* instance;

    SDL_Renderer* renderer = NULL;
    SDL_Window* window = NULL;
    SDL_Event event = SDL_Event{};

    Uint8 fpsCap = 60;
    const Uint8 minFPS = 10;

    bool quit = false;
    bool paused = false;

   private:
    void Loop();

    void HandleFPS(float startTick);
    void HandleKeyboardEvents(SDL_Event* event);
    void HandlePhysics();

    void DrawText(const std::string& text);
};

//------------------------------------------------------------------------------

#endif
