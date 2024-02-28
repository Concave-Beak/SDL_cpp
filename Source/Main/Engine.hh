#ifndef __ENGINE_HEADER__
#define __ENGINE_HEADER__

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

#include "../Entities/Player.hh"

//------------------------------------------------------------------------------

class Engine {
   public:
    bool quit = false;

   public:
    SDL_Window* GetWindow();  // not implemented
    static Engine* GetEngineInstance();
    void Run();

   private:
    static Player* playerInstance;
    static Engine* instance;
    SDL_Renderer* renderer = NULL;

    const Uint16 fpsCap = 60;
    SDL_Event event = SDL_Event{};

   private:
    void Loop();
    void Render();

    void HandleFPS(Uint64 firstTick);
    void HandleKeyboardEvents(SDL_Event* event);
    void HandlePhysics();
};

//------------------------------------------------------------------------------

#endif
