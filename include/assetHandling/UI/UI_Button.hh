#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>

#include <string>
#include <vector>

#include "../../../lib/utils/error_handling.hh"

namespace UI {

// BUTTON CLASS AND FUNCIONS NEED TO BE DONE
// TODO

/*
 * ButtonType enum, used to define a type for a Button, can be used multiple times
 * in a single button.
 */
enum ButtonFlags {
    TEXTURE_BUTTON = 0x01,
    TEXT_BUTTON = 0x02,
    SWITCH_BUTTON = 0x04,
};

/*
 * Button struct, used in UI. Once it's been created it gets pushed to a vector
 * to get drawn. Check DrawButton() function for more info
 */
class Button {
   public:
    Button(ButtonFlags type_, SDL_Rect grid_, std::string ID_);
    Button(ButtonFlags type_, SDL_Rect grid_, std::string ID_, SDL_Color outCol, SDL_Color fillCol, SDL_Color hovCol, SDL_Color textCol);
    ~Button();

    void SetColor(SDL_Color& color, const Uint8& textureField);
    void SetText(std::string& text_);
    const Error SetTexture(SDL_Renderer* renderer, const Uint8 textureField, std::string path);

    static const Error DrawButtons(SDL_Renderer* renderer);

   private:
    Uint8 flags;

    SDL_Rect grid = {};

    SDL_Color fillColor = {};
    SDL_Color outlineColor = {};
    SDL_Color textColor = {};
    SDL_Color hoverColor = {};

    SDL_Texture* defaultTexture = NULL;
    SDL_Texture* hoverTexture = NULL;
    SDL_Texture* clickedTexture = NULL;

    bool isShown = false;

    bool isClicked = false;
    bool isHovered = false;

    std::string text;

    std::string ID;
};

inline static std::vector<Button*> buttonVector{};
}  // namespace UI
