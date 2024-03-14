#ifndef __CONFIG_HEADER__
#define __CONFIG_HEADER__

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

#endif

// TO BE IMPLEMENTED
