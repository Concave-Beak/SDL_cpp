#include "../../include/main/Config.hh"
#include <INIReader.h>
#include <algorithm>
#include <cstdio>

bool Config::ShowFPSState() { return showFPS; }

Config* Config::config = new Config{};
Config* Config::GetConfig() { return config; };

int Config::ReadConfig() {
    INIReader reader("./doc/exampleconfig.ini");

    if (reader.ParseError() < 0) {
        printf("ERROR: Cannot load config file");
        return 1;
    }

    {  // Graphics
        std::string windowMode_ =
            reader.GetString("Graphics", "window-mode", "windowed");
        std::transform(windowMode_.begin(), windowMode_.end(),
                       windowMode_.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        config->windowMode = WINDOWED;
        if (windowMode_ == "fullscreen") {
            config->windowMode = FULLSCREEN;
        }
        if (windowMode_ == "windowed") {
            config->windowMode = WINDOWED;
        }
        if (windowMode_ == "fullscreen-windowed") {
            config->windowMode = FULLSCREEN_WINDOWED;
        }

        std::string resolution_ =
            reader.GetString("Graphics", "resolution", "1024x768");

        reader.GetBoolean("Graphics", "vsync-enabled", false);
    }

    return 0;
}
