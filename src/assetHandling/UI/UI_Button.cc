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
    this->clickedColor = SDL_Color{};
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
    this->clickedColor = fillCol_;
    this->hoverColor = hovCol_;

    buttonVector.push_back(this);
}
Button::~Button() {}

/*
 * @param color The color to be applyed to the field
 * @param colorField The field to apply the color in.
 */
void Button::SetColor(SDL_Color& color_, const Uint8& textureField) {
    switch (textureField) {
        case FILL_COLOR: {
            this->clickedColor = color_;
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
            this->clickedColor = color_;
            break;
        }
    }
}

/*
 * @param text_ The text to be set
 */
void Button::SetText(std::string& text_) {
    this->text = text_;
    if (!(this->flags & TEXT_BUTTON)) this->flags |= TEXT_BUTTON;  // NOTE: this probably doesn't work lol
}

/*
 * @param renderer The renderer to create the button texture
 * @param textureField The field to apply the texture in
 * @param path The path to the image to be turned into a texture
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
    this->flags |= TEXTURE_BUTTON;

    return Error{};
}

void Button::SetFlags(int flags_) { flags = flags_; };

const Error Button::DrawTextureBtn(SDL_Renderer* renderer, const Button* btn) {
    SDL_Texture* textureToRender = nullptr;

    if (btn->flags & SWITCH_BUTTON) {
        textureToRender = (btn->isClicked && btn->clickedTexture != nullptr) ? btn->clickedTexture : btn->defaultTexture;
    } else {
        textureToRender = btn->clickedTexture;
    }

    if (textureToRender != nullptr) {
        scc(SDL_RenderCopy(renderer, textureToRender, nullptr, &btn->grid));
    }

    return Error{};
}

/*
 * Checks the buttonVector for every button and draws it. Should be simple to understand just from reading the code
 */
const Error Button::DrawButtons(SDL_Renderer* renderer) {  // TODO: remove elseifs
    for (const Button* btn : buttonVector) {
        if (!btn->isShown) {
            continue;
        }
        if (btn->flags & TEXTURE_BUTTON) {
            DrawTextureBtn(renderer, btn);
        }
        if (btn->flags & TEXT_BUTTON) {
            // RenderTextSized(renderer, Font *font, const char *text, size_t text_size, Vector2<float> pos, SDL_Color color, float scale)
        }
    }
    return Error{};
}

void Button::HandleButtonClicks(Vector2<int> mousePos) {
    for (Button* btn : buttonVector) {
        if (btn->isShown) {
            if (btn->grid.x < mousePos.x &&
                btn->grid.x + btn->grid.w > mousePos.x &&
                btn->grid.y < mousePos.y &&
                btn->grid.y + btn->grid.h > mousePos.y) {
                if (btn->flags & SWITCH_BUTTON) {
                    btn->isClicked = !btn->isClicked;
                }
                break;  // Button handled, exit loop
            }
        }
    }
}

void Button::ToggleIsShown() { isShown = !isShown; }

//------------------------------------------------------------------------------
