#include "../../include/main/Config.hh"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include <cstdio>
#include <string>

#define TOML_EXCEPTIONS 0
#include "../../lib/tomlplusplus/tomlplusplus.hh"
#include "../../lib/utils/engine_utils.hh"
#include "../../lib/utils/error_handeling.hh"

bool Config::ShowFPSState() { return showFPS; }

Config* Config::config = new Config{};
Config* Config::GetConfig() { return config; };

Vector2<int> Config::GetWindowResolution() { return windowResolution; }

void Config::ReadConfig() {
    std::string path = "./doc/exampleconfig.toml";

    toml::parse_result result;

    result = toml::parse_file(path);
    if (!result) {
        ThrowError(COULDNT_PARSE_CONFIG_FILE, path + " opting for default configuration\n", MEDIUM, logPath);
    }
    toml::table table = std::move(result.table());

    {
        toml::node_view<toml::node> debug = table["Debug"];

        if (debug["debug_enabled"] == true) {
            debugMode = true;
        }
        if (debug["log_file_path"] && debugMode) {
            logPath = *debug["log_file_path"].value<std::string>();  // NOTE: Not used, need to do later
        }
        if (debug["show_fps"] == true && debugMode) {
            showFPS = true;
        }
        if (debug["show_debug_info"] == true && debugMode) {
            showDebugInfo = true;
        }
    }
    {
        toml::node_view<toml::node> graphics = table["Graphics"];

        // Window Mode
        if (graphics["window_mode"].value<std::string>() == "fullscreen") {
            fullscreen = true;
        }

        if (graphics["fullscreen_mode"].value<std::string>() == "fs-default") {
            fullscreenMode = FullscreenMode::FULLSCREEN_DEFAULT;
        }
        if (graphics["fullscreen_mode"].value<std::string>() == "fs-desktop") {
            fullscreenMode = FullscreenMode::FULLSCREEN_DESKTOP;
        }
        if (graphics["fullscreen_mode"].value<std::string>() == "fs-windowed") {
            fullscreenMode = FullscreenMode::FULLSCREEN_WINDOWED;
        }

        // Resolution
        if (graphics["resolution"]) {
            windowResolution.x = *graphics["resolution"][0].value<int>();
            windowResolution.y = *graphics["resolution"][1].value<int>();
        }
    }
}

void Config::ApplyConfig() {
}
