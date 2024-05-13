#pragma once

#include <SDL2/SDL.h>

#include <string>

#include "../../lib/utils/engine_utils.hh"
#include "../../lib/utils/error_handling.hh"
#include "../../lib/utils/math_utils.hh"

class Config {
   public:
    static Config* GetConfig();

    void WriteConfig();
    const Error ApplyConfig(SDL_Window* window, SDL_Renderer* renderer, Vector2<int*> screenResolution);
    void DrawConfigMenu(SDL_Window* window, SDL_Renderer* renderer);

    bool ShowFPSState();
    void ToggleConfigMenu(SDL_Window* window, SDL_Renderer* renderer);

   private:
    bool isConfigMenuToggled = false;

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
    const Error ReadConfig();
};
