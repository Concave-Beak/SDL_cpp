#include "../../../include/assetHandling/UI/UI_Button.hh"

#include <SDL2/SDL_render.h>

#include <string>
#include <string_view>

#include "../../../lib/utils/sdl_utils.hh"

//------------------------------------------------------------------------------
using namespace UI;

Button::Button(ButtonType type_, SDL_Rect grid_, std::string ID_) : grid(grid_) {
    if (ID_ == std::string{}) {
        Crash(Error{BAD_PARAMS, "Could not create button\n", Severity::HIGH});
        return;
    }
    this->ID = ID_;
    this->type = type_;

    this->textColor = SDL_Color{};
    this->outlineColor = SDL_Color{};
    this->fillColor = SDL_Color{};
    this->hoverColor = SDL_Color{};
}
Button::~Button() {}

/*
 * \param color The color to be applyed to the field
 * \param textureField The field to apply the texture in: fillColor, outlineColor, textColor, hovercolor from 0-3
 */
void Button::SetColor(SDL_Color& color_, const Uint8& textureField) {
    switch (textureField) {
        case 0: {
            this->fillColor = color_;
            break;
        }
        case 1: {
            this->outlineColor = color_;
            break;
        }
        case 2: {
            this->textColor = color_;
            break;
        }
        case 3: {
            this->hoverColor = color_;
            break;
        }
        default: {
            this->fillColor = color_;
            break;
        }
    }
}
/*
 * \param text The text to be set
 */
void Button::SetText(std::string& text_) {
    if (text_ == "") return;
    this->text = text_;
}

/*
 * \param renderer The renderer to create the button texture
 * \param textureField The field to apply the texture in: defaultTexture, hoverTexture,clickedTexture from 0-2
 * \param path The path to the image to be turned into a texture
 */
const Error Button::SetTexture(SDL_Renderer* renderer, const Uint8 textureField, std::string path) {
    SDL_Surface* buttonSurface = SurfaceFromFile(path);
    SDL_Texture* buttonTexture = (SDL_Texture*)scp(SDL_CreateTextureFromSurface(renderer, buttonSurface));
    if (buttonTexture == NULL) {
        return Error{COULDNT_LOAD_TEXTURE, path, MEDIUM};
    }
    switch (textureField) {
        case 0: {
            if (this->clickedTexture != nullptr) {
                return Error{TEXTURE_ALREADY_SET, "Clicked texture field is already set", LOW};
            }
            this->defaultTexture = buttonTexture;
            break;
        }
        case 1: {
            if (this->clickedTexture != nullptr) {
                return Error{TEXTURE_ALREADY_SET, "Clicked texture field is already set", LOW};
            }
            this->hoverTexture = buttonTexture;
            break;
        }
        case 2: {
            if (this->clickedTexture != nullptr) {
                return Error{TEXTURE_ALREADY_SET, "Clicked texture field is already set", LOW};
            }
            this->clickedTexture = buttonTexture;
            break;
        }
        default: {
            if (this->clickedTexture != nullptr) {
                return Error{TEXTURE_ALREADY_SET, "Clicked texture field is already set", LOW};
            }
            this->defaultTexture = buttonTexture;
            break;
        }
    }
    if (this->type % TEXTURE_BUTTON != 0) {
        this->type |= TEXTURE_BUTTON;
    }
    return Error{};
}

//------------------------------------------------------------------------------
