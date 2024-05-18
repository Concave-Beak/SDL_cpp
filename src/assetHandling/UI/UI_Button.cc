#include "../../../include/assetHandling/UI/UI_Button.hh"

#include <SDL2/SDL_render.h>

#include <string>

#include "../../../lib/utils/sdl_utils.hh"

//------------------------------------------------------------------------------
using namespace UI;

Button::Button(ButtonFlags type_, SDL_Rect grid_, std::string ID_) {
    if (ID_.empty()) Crash(Error(BAD_PARAMS, "Could not create button\n", Severity::HIGH));

    this->ID = ID_;
    this->flags = type_;
    this->grid = grid_;

    this->textColor = SDL_Color{};
    this->outlineColor = SDL_Color{};
    this->fillColor = SDL_Color{};
    this->hoverColor = SDL_Color{};

    buttonVector.push_back(this);
}

Button::Button(ButtonFlags type_, SDL_Rect grid_, std::string ID_, SDL_Color textCol_, SDL_Color outCol_, SDL_Color fillCol_, SDL_Color hovCol_) {
    if (ID_.empty()) Crash(Error(BAD_PARAMS, "Could not create button\n", Severity::HIGH));

    this->ID = ID_;
    this->flags = type_;
    this->grid = grid_;

    this->textColor = textCol_;
    this->outlineColor = outCol_;
    this->fillColor = fillCol_;
    this->hoverColor = hovCol_;

    buttonVector.push_back(this);
}
Button::~Button() {}

/*
 * \param color The color to be applyed to the field
 * \param colorField The field to apply the color in.
 */
void Button::SetColor(SDL_Color& color_, const Uint8& textureField) {
    switch (textureField) {
        case FILL_COLOR: {
            this->fillColor = color_;
            break;
        }
        case OUTLINE_COLOR: {
            this->outlineColor = color_;
            break;
        }
        case TEXT_COLOR: {
            this->textColor = color_;
            break;
        }
        case HOVER_COLOR: {
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
 * \param text_ The text to be set
 */
void Button::SetText(std::string& text_) {
    this->text = text_;
    if (this->flags / TEXT_BUTTON == 0) this->flags |= TEXT_BUTTON;
}

/*
 * \param renderer The renderer to create the button texture
 * \param textureField The field to apply the texture in
 * \param path The path to the image to be turned into a texture
 */
const Error Button::SetTexture(SDL_Renderer* renderer, const TextureField& textureField, std::string path) {
    SDL_Surface* buttonSurface = SurfaceFromFile(path);
    SDL_Texture* buttonTexture = (SDL_Texture*)scp(SDL_CreateTextureFromSurface(renderer, buttonSurface));
    if (buttonTexture == NULL) {
        return Error{COULDNT_LOAD_TEXTURE, path, MEDIUM};
    }
    switch (textureField) {
        case DEFAULT_TEXTURE: {
            if (this->defaultTexture != nullptr) {
                return Error{TEXTURE_ALREADY_SET, "Clicked texture field is already set", LOW};
            }
            this->defaultTexture = buttonTexture;
            break;
        }
        case HOVER_TEXTURE: {
            if (this->hoverTexture != nullptr) {
                return Error{TEXTURE_ALREADY_SET, "Clicked texture field is already set", LOW};
            }
            this->hoverTexture = buttonTexture;
            break;
        }
        case CLICKED_TEXTURE: {
            if (this->clickedTexture != nullptr) {
                return Error{TEXTURE_ALREADY_SET, "Clicked texture field is already set", LOW};
            }
            this->clickedTexture = buttonTexture;
            break;
        }
        default: {
            if (this->defaultTexture != nullptr) {
                return Error{TEXTURE_ALREADY_SET, "Clicked texture field is already set", LOW};
            }
            this->defaultTexture = buttonTexture;
            break;
        }
    }
    if (this->flags / TEXTURE_BUTTON == 0) this->flags |= TEXTURE_BUTTON;

    return Error{};
}

/*
 * Checks the buttonVector for every button and draws it. Should be simple to understand just from reading the code
 */
const Error Button::DrawButtons(SDL_Renderer* renderer) {  // TODO: remove elseifs
    for (const Button* btn : buttonVector) {
        if (btn->isShown) {
            if (btn->flags & TEXTURE_BUTTON) {
                if (btn->defaultTexture != NULL) {
                    scc(SDL_RenderCopy(renderer, btn->defaultTexture, NULL, &btn->grid));
                }
            } else {
            }
            if (btn->flags & TEXT_BUTTON) {
            }
            if (btn->flags & SWITCH_BUTTON) {
            }
        }
    }
    return Error{};
}

void Button::HandleButtons(SDL_Event event) {
    for (const Button* btn : buttonVector) {
        if (btn->isShown) {
            if (event.type == SDL_MOUSEBUTTONDOWN) {
            }
        }
    }
}

void Button::ToggleIsShown() {
    isShown = !isShown;
}

//------------------------------------------------------------------------------
