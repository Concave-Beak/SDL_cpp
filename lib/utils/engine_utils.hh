#pragma once
#include <limits.h>
#include <pwd.h>
#include <unistd.h>

#include <cstdlib>
#include <string>

#define ENTITY_MAX 1024

#define GRAVITY 120.0f

#define MAX_X_SPEED 400
#define MAX_Y_SPEED 400

#define MAX_PROJECTILE_SPEED_X 400
#define MAX_PROJECTILE_SPEED_Y 400

#define AIR_ATTRITION 1.5f
#define SURFACE_BOUNCE 0.2f

enum class Direction {
    LEFT = 0,
    RIGHT,
    UP,
    DOWN,

};

enum class FullscreenMode {
    FULLSCREEN_DEFAULT = 0,
    FULLSCREEN_WINDOWED,
    FULLSCREEN_DESKTOP,
};

inline std::string GetAbsolutePath(const std::string& relative_path) {
    std::string expanded_path = relative_path;

    // Expand `~` to the home directory path
    if (!relative_path.empty() && relative_path[0] == '~') {
        const char* home = getenv("HOME");
        if (home || (home = getpwuid(getuid())->pw_dir)) {
            expanded_path = std::string(home) + relative_path.substr(1);
        } else {
            return "";
        }
    }

    char abs_path[PATH_MAX];

    if (realpath(expanded_path.c_str(), abs_path) != nullptr) {
        return std::string(abs_path);
    } else {
        return "";
    }
}
