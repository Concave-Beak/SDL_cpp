#include "../../../include/assetHandling/UI/UI_Button.hh"

#include <SDL2/SDL_render.h>

#include <cstdio>
#include <functional>
#include <string>

#include "../../../lib/utils/math_utils.hh"
#include "../../../lib/utils/sdl_utils.hh"

//------------------------------------------------------------------------------

using namespace UI;

Button::Button(ButtonFlags flags_, SDL_Rect grid_) {
    this->flags = flags_;
    this->grid = grid_;

    this->outlineColor = SDL_Color{};

    buttonVector.push_back(this);
}
Button::~Button() {}
void Button::SetOutlineColor(SDL_Color& color_) {}

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

    return Error{};
}

void Button::SetFlags(int flags_) { flags = flags_; };

const Error Button::DrawTextureBtn(SDL_Renderer* renderer, const Button* btn) {
    SDL_Texture* textureToRender = nullptr;

    if (btn->flags & SWITCH_BUTTON) {
        textureToRender =
            (btn->isClicked && btn->clickedTexture != nullptr) ? btn->clickedTexture : btn->defaultTexture;
    } else {
        textureToRender = btn->defaultTexture;
    }

    if (textureToRender != nullptr) {
        scc(SDL_RenderCopy(renderer, textureToRender, nullptr, &btn->grid));
    } else {
        DrawTextureNotFound(btn->grid, Vector2<int>{16, 16}, renderer);
        return Error(TEXTURE_IS_NULL, "Could not draw button\n", MEDIUM);
    }

    return Error{};
}

const Error Button::DrawButtons(SDL_Renderer* renderer) {
    Error err{};
    for (Button* btn : buttonVector) {
        if (!btn->isShown) {
            continue;
        }
        SDL_Texture* textureToRender = nullptr;

        if (btn->flags & SWITCH_BUTTON) {
            textureToRender = (btn->isClicked && btn->clickedTexture != nullptr) ? btn->clickedTexture
                                                                                 : btn->defaultTexture;
        } else {
            textureToRender = btn->clickedTexture;
        }

        if (textureToRender != nullptr) {
            scc(SDL_RenderCopy(renderer, textureToRender, nullptr, &btn->grid));
        } else {
            err = Error(TEXTURE_IS_NULL, "Could not draw button\n", MEDIUM);
            DrawTextureNotFound(btn->grid, Vector2<int>{16, 16}, renderer);
        }
    }
    return err;
}

void Button::HandleButtonClicks(Vector2<int> mousePos) {
    for (Button* btn : buttonVector) {
        if (btn->isShown) {
            if (btn->grid.x < mousePos.x && btn->grid.x + btn->grid.w > mousePos.x &&
                btn->grid.y < mousePos.y && btn->grid.y + btn->grid.h > mousePos.y) {
                if (btn->flags & SWITCH_BUTTON) {
                    btn->isClicked = !btn->isClicked;
                }
                btn->clickEvent();
                break;  // Button handled, exit loop
            }
        }
    }
}

void Button::ToggleIsShown() { isShown = !isShown; }

//------------------------------------------------------------------------------
