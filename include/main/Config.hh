#pragma once

#include <SDL2/SDL.h>

#include <string>
#include <unordered_map>

#include "../../lib/utils/engine_utils.hh"
#include "../../lib/utils/error_handling.hh"
#include "../../lib/utils/math_utils.hh"
#include "./Action.hh"

#define TOML_EXCEPTIONS 0  // to remove try/catch statmement
#include "../../lib/tomlplusplus/tomlplusplus.hh"

class Config {
   public:
    static Config* GetConfig();

    void WriteConfig();  // TODO
    const Error ApplyConfig(SDL_Window* window, SDL_Renderer* renderer, Vec2<int*> screenResolution_, ActionHandler* actionHandler);
    bool ShowFPSState();          // TODO: SHOULD BE REMOVED
    void ToggleMenuVisibility();  // TODO

   private:
    bool isConfigMenuVisible = false;

    // Graphics
    bool fullscreen = false;
    FullscreenMode fullscreenMode = FullscreenMode::FULLSCREEN_WINDOWED;
    Vec2<int> screenResolution = {1024, 768};

    // Debug
    bool debugMode = false;
    std::string logPath = "./logs/debug.log";
    bool showFPS = false;
    bool showDebugInfo = false;

    static Config* config;

   private:
    static std::unordered_map<std::string, Key> str2Keybinding;
    static std::unordered_map<std::string, Action::ActionType> actionTypeMap;

   private:
    const Error ReadFullConfig(ActionHandler* actionHandler);

    const Error ReadGraphics(toml::node_view<toml::node> graphicsNode);
    void SetGraphics();  // TODO

    const Error ReadAudio(toml::node_view<toml::node> audioNode);
    void SetAudio();  // TODO

    const Error ReadDebug(toml::node_view<toml::node> debugNode);
    void SetDebug();  // TODO

    const Error ReadControls(toml::node_view<toml::node> controlsNode);
    void SetControls();  // TODO

    const Error ReadKeybindings(toml::node_view<toml::node> keybindingNode, ActionHandler* actionHandler);
    void SetKeybindings();
};
