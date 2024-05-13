#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>

#include <string>
#include <vector>

#include "../../../lib/utils/error_handling.hh"

namespace UI {

enum ButtonType {
    TEXT_BUTTON = 0x01,
    TEXTURE_BUTTON = 0x02,
    SWITCH_BUTTON = 0x04,
};

class Button {
   public:
    Button(SDL_Rect grid_, std::string ID_);
    Button(SDL_Rect grid_, std::string ID_, SDL_Color outCol, SDL_Color fillCol, SDL_Color hovCol, SDL_Color textCol);
    ~Button();

    void InitColors(SDL_Color outCol, SDL_Color fillCol, SDL_Color hovCol, SDL_Color textCol);

    Error DrawButton();

   private:
    SDL_Rect grid = {};

    SDL_Color outlineColor = {};
    SDL_Color fillColor = {};
    SDL_Color hoverColor = {};
    SDL_Color textColor = {};

    bool isShown = false;

    bool isPressed = false;  // Only used in Switch style buttons
    bool isHovered = false;

    std::string Text;

    std::string ID;
};

inline std::vector<Button> buttonVector{};
}  // namespace UI
