#include "../../include/main/Config.hh"

#include <SDL2/SDL_render.h>

#include <cstdio>
#include <string>

#define TOML_EXCEPTIONS 0
#include "../../lib/tomlplusplus/tomlplusplus.hh"
#include "../../lib/utils/error_handeling.hh"

bool Config::ShowFPSState() { return showFPS; }

Config* Config::config = new Config{};
Config* Config::GetConfig() { return config; };

void Config::ReadConfig() {
    std::string path = "./doc/exampleconfig.tol";

    toml::parse_result result;

    result = toml::parse_file(path);
    if (!result) {
        ThrowError(COULDNT_PARSE_CONFIG_FILE, path + " opting for default configuration\n", MAJOR, logPath);
    }
    toml::table table = std::move(result.table());

    {
        toml::node_view<toml::node> debug = table["Debug"];

        if (debug["debug_enabled"] == true) {
            debugMode = true;
        }
        if (debug["log_file_path"]) {
            logPath = *debug["log_file_path"].value<std::string>();
        }
        if (debug["show_fps"] == true) {
            showFPS = true;
        }
        if (debug["show_debug_info"] == true) {
            showDebugInfo = true;
        }
    };
    {
        toml::node_view<toml::node> graphics = table["Graphics"];

        // Window Mode
        if (graphics["window_mode"].value<std::string>() == "fullscreen") {
            windowMode = FULLSCREEN;
        }
        if (graphics["window_mode"].value<std::string>() == "fullscreen-windowed") {
            windowMode = FULLSCREEN_WINDOWED;
        }
        if (graphics["window_mode"].value<std::string>() == "windowed") {
            windowMode = WINDOWED;
        }

        // Resolution
        if (graphics["resolution"]) {
        }
    }
}
