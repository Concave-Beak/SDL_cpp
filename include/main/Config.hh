#pragma once

class Config {
   public:
    static Config* GetConfig();
    bool ShowFPSState();
    bool fullscreen = false;

   private:
    bool showFPS = true;
    bool fakeFullscreen = false;
    static Config* config;
};

// TO BE IMPLEMENTED
