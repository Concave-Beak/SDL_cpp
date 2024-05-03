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

    bool ShowFPSState();
    bool fullscreen = false;

   private:
    Uint32 windowFlags;
    Uint32 rendererFlags;

    // Graphics
    WindowMode windowMode = WINDOWED;
    Vector2<float> windowResolution = {1024.0f, 768.0f};

    // Debug
    bool debugMode = false;
    std::string logPath = "./logs/debug.log";
    bool showFPS = false;
    bool showDebugInfo = false;

    static Config* config;
};

// TO BE IMPROVED
