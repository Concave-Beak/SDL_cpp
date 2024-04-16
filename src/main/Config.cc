#include "../../include/main/Config.hh"

bool Config::ShowFPSState() {
    return showFPS;
}

Config* Config::config = new Config{};
Config* Config::GetConfig() {
    return config;
};
