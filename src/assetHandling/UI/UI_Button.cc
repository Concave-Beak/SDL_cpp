#include "../../../include/assetHandling/UI/UI_Button.hh"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>

#include <string>

#include "../../../lib/utils/sdl_utils.hh"

namespace UI {

std::vector<Button*> Button::buttonVector;

Button::Button(ButtonFlags flags_, SDL_Rect grid_)
    : flags(flags_),
      grid(grid_),
      outlineColor({0, 0, 0, 0}),
      defaultTexture(nullptr),
      hoverTexture(nullptr),
      clickedTexture(nullptr),
      isShown(true),
      isClicked(false),
      isHovered(false) {
    this->AddToButtonVector();
}

Button::~Button() {
    if (defaultTexture) SDL_DestroyTexture(defaultTexture);
    if (hoverTexture) SDL_DestroyTexture(hoverTexture);
    if (clickedTexture) SDL_DestroyTexture(clickedTexture);
}

void Button::SetOutlineColor(const SDL_Color& color_) { outlineColor = color_; }

const Error Button::AssignTexture(SDL_Texture*& oldTexture, SDL_Texture* newTexture) {
    if (oldTexture) {
        return Error{TEXTURE_ALREADY_SET, "Texture field is already set", LOW};
    }
    oldTexture = newTexture;
    return Error{};
}

Error Button::SetTexture(SDL_Renderer* renderer, TextureField textureField, const std::string& path) {
    SDL_Surface* buttonSurface = SurfaceFromFile(path);
    SDL_Texture* newButtonTexture = static_cast<SDL_Texture*>(scp(SDL_CreateTextureFromSurface(renderer, buttonSurface)));
    if (!newButtonTexture) {
        return Error{COULDNT_LOAD_TEXTURE, path, MEDIUM};
    }

    switch (textureField) {
        case DEFAULT_TEXTURE:
            return AssignTexture(defaultTexture, newButtonTexture);
        case HOVER_TEXTURE:
            return AssignTexture(hoverTexture, newButtonTexture);
        case CLICKED_TEXTURE:
            return AssignTexture(clickedTexture, newButtonTexture);
        default:
            return Error{};
    }
}

Error Button::SetTexture(SDL_Texture* newTexture, TextureField textureField) {
    switch (textureField) {
        case DEFAULT_TEXTURE:
            return AssignTexture(defaultTexture, newTexture);
        case HOVER_TEXTURE:
            return AssignTexture(hoverTexture, newTexture);
        case CLICKED_TEXTURE:
            return AssignTexture(clickedTexture, newTexture);
        default:
            return Error{};
    }
    return Error();
}

void Button::SetFlags(ButtonFlags flags_) { this->flags = flags_; }

void Button::SetFunction(std::function<Error()> clickEvent_) {
    this->clickEvent = std::move(clickEvent_);
}

void Button::AddToButtonVector() { buttonVector.emplace_back(this); }

Error Button::Handle(SDL_Event event, SDL_Renderer* renderer) {
    SDL_PollEvent(&event);  // TODO: Remove this

    Error err = DrawButtons(renderer);
    if (!err.IsNull()) {
        return err;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        HandleClicks(Vec2<int>{event.button.x, event.button.y});
    }

    if (event.type == SDL_MOUSEMOTION) {
        lastMousepos = {event.motion.x, event.motion.y};
    }
    HandleHover(lastMousepos, renderer);

    return err;
}

Error Button::DrawButtons(SDL_Renderer* renderer) {
    for (const auto& btn : buttonVector) {
        if (!btn->isShown || btn->isHovered) continue;

        SDL_Texture* textureToRender = btn->defaultTexture;
        if (btn->flags == SWITCH_BUTTON && btn->isClicked) {
            textureToRender = btn->clickedTexture;
        }

        if (textureToRender) {
            scc(SDL_RenderCopy(renderer, textureToRender, nullptr, &btn->grid));
        } else {
            DrawTextureNotFound(btn->grid, {16, 16}, renderer);
            return Error(TEXTURE_IS_NULL, "Could not draw button\n", MEDIUM);
        }
    }
    return Error();
}

void Button::HandleClicks(Vec2<int> mousePos) {
    SDL_Point mousePoint{.x = mousePos.x, .y = mousePos.y};
    for (const auto& btn : buttonVector) {
        if (btn->isShown && SDL_PointInRect(&mousePoint, &btn->grid)) {
            if (btn->flags == SWITCH_BUTTON) {
                btn->isClicked = !btn->isClicked;
            }
            if (btn->clickEvent) {
                btn->clickEvent();
            }
            break;  // Button handled, exit loop
        }
    }
}
void Button::HandleHover(Vec2<int> mousePos, SDL_Renderer* renderer) {
    SDL_Point mousePoint{.x = mousePos.x, .y = mousePos.y};
    for (const auto& btn : buttonVector) {
        if (btn->isShown && SDL_PointInRect(&mousePoint, &btn->grid)) {
            if (btn->hoverTexture) {
                scc(SDL_RenderCopy(renderer, btn->hoverTexture, nullptr, &btn->grid));
            } else {
                DrawTextureNotFound(btn->grid, Vec2<int>{16, 16}, renderer);
            }
            btn->isHovered = true;
            break;  // Button handled, exit loop
        } else {
            btn->isHovered = false;
        }
    }
}

void Button::ToggleIsShown() {
    isShown = !isShown;
}

}  // namespace UI
