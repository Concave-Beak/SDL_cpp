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

Vector2<int> Config::GetScreenResolution() { return screenResolution; }

Uint32 Config::GetWindowFlags() { return windowFlags; };

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
            screenResolution.x = *graphics["resolution"][0].value<int>();
            screenResolution.y = *graphics["resolution"][1].value<int>();
        }
    }
}

void Config::ApplyConfig(SDL_Window* window, SDL_Renderer* renderer, Vector2<int*> screenResolution_) {
    ReadConfig();

    *screenResolution_.x = screenResolution.x;
    *screenResolution_.y = screenResolution.y;

    if (window == NULL || renderer == NULL) {
        return;
    }

    windowFlags &= SDL_WINDOW_FULLSCREEN;
    windowFlags &= SDL_WINDOW_FULLSCREEN_DESKTOP;

    if (fullscreen) {
        if (fullscreenMode == FULLSCREEN_WINDOWED) {
            SDL_DisplayMode mode;
            if (SDL_GetDesktopDisplayMode(0, &mode) != 0) {
                ThrowError(SDL_FUNCTION_ERROR,
                           "Couldn't get DisplayMode, couldn't go fullscreen windowed\n",
                           MEDIUM, logPath);
            } else {
                SDL_SetWindowSize(window, mode.w, mode.h);
                SDL_SetWindowPosition(window, 0, 0);
                *screenResolution_.x = mode.w;
                *screenResolution_.y = mode.h;
            }
        }
        if (fullscreenMode == FULLSCREEN_DEFAULT) {
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        }
        if (fullscreenMode == FULLSCREEN_DESKTOP) {
            windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
    }
    SDL_SetWindowFullscreen(window, windowFlags);
}
