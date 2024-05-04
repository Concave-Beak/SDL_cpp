#pragma once

#include <SDL2/SDL.h>

#include <string>

#include "../../lib/utils/engine_utils.hh"
#include "../../lib/utils/math_utils.hh"

class Config {
   public:
    static Config* GetConfig();

    void WriteConfig();
    void ApplyConfig(SDL_Window* window, SDL_Renderer* renderer, Vector2<int*> screenResolution);

    Vector2<int> GetScreenResolution();
    Uint32 GetWindowFlags();

    bool ShowFPSState();

   private:
    Uint32 windowFlags = SDL_WINDOW_BORDERLESS;
    Uint32 rendererFlags;

    // Graphics
    bool fullscreen = false;
    FullscreenMode fullscreenMode = FULLSCREEN_WINDOWED;
    Vector2<int> screenResolution = {1024, 768};

    // Debug
    bool debugMode = false;
    std::string logPath = "./logs/debug.log";
    bool showFPS = false;
    bool showDebugInfo = false;

    static Config* config;

   private:
    void ReadConfig();
};
