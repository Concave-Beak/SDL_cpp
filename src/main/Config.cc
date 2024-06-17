#include "../../include/main/Config.hh"

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

#include <cstdio>
#include <string>

#include "../../lib/tomlplusplus/tomlplusplus.hh"
#include "../../lib/utils/engine_utils.hh"

//------------------------------------------------------------------------------

std::unordered_map<std::string, Key> Config::str2Keybinding = {
    {"A", {SDL_SCANCODE_A, 0}},
    {"B", {SDL_SCANCODE_B, 0}},
    {"C", {SDL_SCANCODE_C, 0}},
    {"D", {SDL_SCANCODE_D, 0}},
    {"E", {SDL_SCANCODE_E, 0}},
    {"F", {SDL_SCANCODE_F, 0}},
    {"G", {SDL_SCANCODE_G, 0}},
    {"H", {SDL_SCANCODE_H, 0}},
    {"I", {SDL_SCANCODE_I, 0}},
    {"J", {SDL_SCANCODE_J, 0}},
    {"K", {SDL_SCANCODE_K, 0}},
    {"L", {SDL_SCANCODE_L, 0}},
    {"M", {SDL_SCANCODE_M, 0}},
    {"N", {SDL_SCANCODE_N, 0}},
    {"O", {SDL_SCANCODE_O, 0}},
    {"P", {SDL_SCANCODE_P, 0}},
    {"Q", {SDL_SCANCODE_Q, 0}},
    {"R", {SDL_SCANCODE_R, 0}},
    {"S", {SDL_SCANCODE_S, 0}},
    {"T", {SDL_SCANCODE_T, 0}},
    {"U", {SDL_SCANCODE_U, 0}},
    {"V", {SDL_SCANCODE_V, 0}},
    {"W", {SDL_SCANCODE_W, 0}},
    {"X", {SDL_SCANCODE_X, 0}},
    {"Y", {SDL_SCANCODE_Y, 0}},
    {"Z", {SDL_SCANCODE_Z, 0}},
    {"ESC", {SDL_SCANCODE_ESCAPE}},

    {"MOUSE1", {SDL_SCANCODE_UNKNOWN, SDL_BUTTON_LEFT}},
    {"MOUSE2", {SDL_SCANCODE_UNKNOWN, SDL_BUTTON_RIGHT}},
};

std::unordered_map<std::string, Action::ActionType> Config::actionTypeMap = {
    {"move_up", Action::ActionType::MOVE_UP},
    {"move_down", Action::ActionType::MOVE_DOWN},
    {"move_left", Action::ActionType::MOVE_LEFT},
    {"move_right", Action::ActionType::MOVE_RIGHT},

    {"attack1", Action::ActionType::ATTACK1},
    {"attack2", Action::ActionType::ATTACK2},

    {"switch_weapons", Action::ActionType::SWITCH_WEAPONS},
    {"open_inventory", Action::ActionType::OPEN_INVENTORY},

    {"quit_game", Action::ActionType::QUIT_GAME},
};

bool Config::ShowFPSState() { return showFPS; }

Config* Config::config = new Config{};
Config* Config::GetConfig() { return config; };

std::string Config::GetLogPath() { return logPath; }

const Error Config::ReadFullConfig(ActionHandler* actionHandler) {
    std::string path = GetAbsolutePath("~/.config/Soulbound/config.toml");

    toml::parse_result result;

    result = toml::parse_file(path);
    if (!result) {
        return Error(Error::ErrorCode::COULDNT_PARSE_CONFIG_FILE, path + " opting for default configuration\n", Error::Severity::MEDIUM);
    }
    toml::table table = std::move(result.table());

    Error err = ReadDebug(table["Debug"]);
    if (!err.IsEmpty()) {
        return err;
    }

    err = ReadGraphics(table["Graphics"]);
    if (!err.IsEmpty()) {
        return err;
    }
    err = ReadKeybindings(table["Keybindings"], actionHandler);
    if (!err.IsEmpty()) {
        return err;
    }

    return Error{};
}

const Error Config::ApplyConfig(SDL_Window* window, SDL_Renderer* renderer, Vec2<int*> screenResolution_, ActionHandler* actionHandler) {
    Error err = ReadFullConfig(actionHandler);
    if (!err.IsEmpty()) return err;
    (void)renderer;  // TODO

    *screenResolution_.x = screenResolution.x;
    *screenResolution_.y = screenResolution.y;
    SDL_SetWindowFullscreen(window, SDL_WINDOW_BORDERLESS);

    if (window == NULL || renderer == NULL) {
        return Error(Error::ErrorCode::BAD_PARAMS, "Window or renderer are null", Error::Severity::LOW);
    }

    if (fullscreen) {
        if (fullscreenMode == FullscreenMode::FULLSCREEN_DEFAULT) {
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        }
        if (fullscreenMode == FullscreenMode::FULLSCREEN_DESKTOP) {
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        }

        SDL_DisplayMode mode;
        if (SDL_GetDesktopDisplayMode(0, &mode) != 0) {
            return Error(Error::ErrorCode::SDL_FUNCTION_ERROR, "Couldn't get DisplayMode, couldn't go fullscreen windowed\n",
                         Error::Severity::MEDIUM);
        } else {
            SDL_SetWindowSize(window, mode.w, mode.h);
            SDL_SetWindowPosition(window, 0, 0);
            *screenResolution_.x = mode.w;
            *screenResolution_.y = mode.h;
        }
    }
    SDL_SetWindowSize(window, screenResolution.x, screenResolution.y);
    return Error{};
}

void Config::ToggleMenuVisibility() { isConfigMenuVisible = !isConfigMenuVisible; }

const Error Config::ReadDebug(toml::node_view<toml::node> debugNode) {
    debugMode = false, showFPS = false, showDebugInfo = false;

    if (debugNode["debug_enabled"] == true) {
        debugMode = true;
    }
    if (debugNode["log_file_path"] && debugMode) {
        logPath = *debugNode["log_file_path"].value<std::string>();  // NOTE: Not used, need to do later
    }
    if (debugNode["show_fps"] == true && debugMode) {
        showFPS = true;
    }
    if (debugNode["show_debug_info"] == true && debugMode) {
        showDebugInfo = true;
    }
    return Error{};
}

const Error Config::ReadGraphics(toml::node_view<toml::node> graphicsNode) {
    fullscreen = false, fullscreenMode = FullscreenMode::FULLSCREEN_WINDOWED;

    // Window Mode
    if (graphicsNode["window_mode"].value<std::string>() == "fullscreen") {
        fullscreen = true;
    }

    if (graphicsNode["fullscreen_mode"].value<std::string>() == "fs-default") {
        fullscreenMode = FullscreenMode::FULLSCREEN_DEFAULT;
    }
    if (graphicsNode["fullscreen_mode"].value<std::string>() == "fs-desktop") {
        fullscreenMode = FullscreenMode::FULLSCREEN_DESKTOP;
    }
    if (graphicsNode["fullscreen_mode"].value<std::string>() == "fs-windowed") {
        fullscreenMode = FullscreenMode::FULLSCREEN_WINDOWED;
    }

    // Resolution
    if (graphicsNode["resolution"]) {
        screenResolution.x = *graphicsNode["resolution"][0].value<int>();
        screenResolution.y = *graphicsNode["resolution"][1].value<int>();
    }
    return Error{};
}

const Error Config::ReadKeybindings(toml::node_view<toml::node> keybindingNode, ActionHandler* actionHandler) {
    for (std::unordered_map<std::string, Action::ActionType>::iterator it = actionTypeMap.begin(); it != actionTypeMap.end(); ++it) {
        std::string stringKeybinding = *keybindingNode[it->first].value<std::string>();

        if (str2Keybinding.find(stringKeybinding) != str2Keybinding.end()) {
            Action action(it->second);
            actionHandler->SetAction(action, str2Keybinding[stringKeybinding]);
        }
    }
    return Error{};
}
