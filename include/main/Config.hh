#pragma once

#include <SDL2/SDL_stdinc.h>

#include <string>

#include "../../lib/utils/engine_utils.hh"
#include "../../lib/utils/math_utils.hh"

class Config {
   public:
    static Config* GetConfig();

    void WriteConfig();
    void ReadConfig();
    void ApplyConfig();

    Vector2<int> GetWindowResolution();

    bool ShowFPSState();

   private:
    Uint32 windowFlags;
    Uint32 rendererFlags;

    // Graphics
    bool fullscreen = false;
    FullscreenMode fullscreenMode = FULLSCREEN_WINDOWED;
    Vector2<int> windowResolution = {1024, 768};

    // Debug
    bool debugMode = false;
    std::string logPath = "./logs/debug.log";
    bool showFPS = false;
    bool showDebugInfo = false;

    static Config* config;
};

// TO BE IMPROVED
