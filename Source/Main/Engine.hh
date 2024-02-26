#ifndef __ENGINE_HEADER__
#define __ENGINE_HEADER__

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

#include "../Entities/Player.hh"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

class Engine {
   public:
    bool quit = false;

   public:
    SDL_Window* GetWindow();  // not implemented
    static Engine* GetEngineInstance();
    static bool EnableDisableFPS(bool);
    void Init();

   private:
    static Player* player_instance;
    static Engine* instance;
    const Uint32 fps = 60;

   private:
    static SDL_Renderer* renderer;
    void ShowFPS();

    void HandleKeyboardEvents(SDL_Event* event);
};

#endif
