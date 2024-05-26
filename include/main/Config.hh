#pragma once

#include <SDL2/SDL.h>

#include <string>

#include "../../include/assetHandling/UI/UI_Button.hh"
#include "../../lib/utils/engine_utils.hh"
#include "../../lib/utils/error_handling.hh"
#include "../../lib/utils/math_utils.hh"

class Config {
   public:
    static Config* GetConfig();

    void WriteConfig();
    const Error ApplyConfig(SDL_Window* window, SDL_Renderer* renderer, Vector2<int*> screenResolution);
    bool ShowFPSState();
    void ToggleMenuVisibility();

   private:
    bool isConfigMenuVisible = false;

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
