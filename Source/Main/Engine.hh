#ifndef __ENGINE_HEADER__
#define __ENGINE_HEADER__

#include <SDL2/SDL.h>

#include "../Entities/Player.hh"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

class Engine {
   public:
    bool quit = false;

   public:
    static Engine* GetEngineInstance();
    void Init();

   private:
    static Player* player_instance;
    static Engine* instance;

   private:
    void HandleKeyboardEvents(SDL_Event* event);

    SDL_Window* GetWindow();
};

#endif
