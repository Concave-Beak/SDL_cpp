#pragma once

#include "../../lib/utils/math_utils.hh"
#include "../../lib/utils/engine_utils.hh"

class Config {
   public:
    static Config* GetConfig();

    int WriteConfig();
    int ReadConfig();

    bool ShowFPSState();
    bool fullscreen = false;

   private:
    // Graphics
    WindowMode windowMode;
    Vector2<float> windowResolution;

    // Debug
    bool showFPS = true;
    bool fakeFullscreen = false;
    static Config* config;
};

// TO BE IMPROVED
