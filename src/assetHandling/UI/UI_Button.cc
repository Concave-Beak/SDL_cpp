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
    AddToButtonVector();
}

Button::~Button() {
    if (defaultTexture) SDL_DestroyTexture(defaultTexture);
    if (hoverTexture) SDL_DestroyTexture(hoverTexture);
    if (clickedTexture) SDL_DestroyTexture(clickedTexture);
}

void Button::SetOutlineColor(const SDL_Color& color_) { outlineColor = color_; }

const Error Button::AssignTexture(SDL_Texture*& oldTexture, SDL_Texture* newTexture) {
    if (oldTexture) {
        return Error(Error::ErrorCode::TEXTURE_ALREADY_SET, "Texture field is already set", Error::Severity::LOW);
    }
    oldTexture = newTexture;
    return Error();
}

const Error Button::SetTexture(SDL_Renderer* renderer, TextureField textureField, const std::string& path) {
    SDL_Surface* buttonSurface = SurfaceFromFile(path);
    SDL_Texture* newButtonTexture = static_cast<SDL_Texture*>(scp(SDL_CreateTextureFromSurface(renderer, buttonSurface)).Handle().result);
    if (!newButtonTexture) {
        return Error(Error::ErrorCode::COULDNT_LOAD_TEXTURE, path, Error::Severity::MEDIUM);
    }

    switch (textureField) {
        case DEFAULT_TEXTURE:
            return AssignTexture(defaultTexture, newButtonTexture);
        case HOVER_TEXTURE:
            return AssignTexture(hoverTexture, newButtonTexture);
        case CLICKED_TEXTURE:
            return AssignTexture(clickedTexture, newButtonTexture);
        default:
            return Error();
    }
    return Error();
}

const Error Button::SetTexture(SDL_Texture* newTexture, TextureField textureField) {
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

void Button::SetFlags(ButtonFlags flags_) { flags = flags_; }

void Button::SetFunction(std::function<const Error()> clickEvent_) {
    clickEvent = std::move(clickEvent_);
}

void Button::AddToButtonVector() { buttonVector.emplace_back(this); }

const Error Button::Handle(SDL_Event event, SDL_Renderer* renderer) {
    // SDL_PollEvent(&event);  // TODO: Remove this

    Error err = DrawButtons(renderer);
    if (!err.IsEmpty()) {
        return err;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        return HandleClicks(Vec2<int>{event.button.x, event.button.y});
    }

    if (event.type == SDL_MOUSEMOTION) {
        lastMousepos = {event.motion.x, event.motion.y};
    }
    HandleHover(lastMousepos, renderer);

    return err;
}

const Error Button::DrawButtons(SDL_Renderer* renderer) {
    for (const auto& btn : buttonVector) {
        if (!btn->isShown || btn->isHovered) continue;

        SDL_Texture* textureToRender = btn->defaultTexture;
        if (btn->flags == SWITCH_BUTTON && btn->isClicked) {
            textureToRender = btn->clickedTexture;
        }

        if (textureToRender) {
            scc(SDL_RenderCopy(renderer, textureToRender, nullptr, &btn->grid)).Handle();
        } else {
            DrawTextureNotFound(btn->grid, {16, 16}, renderer);
            return Error(Error::ErrorCode::TEXTURE_IS_NULL, "Could not draw button\n", Error::Severity::MEDIUM);
        }
    }
    return Error();
}

const Error Button::HandleClicks(Vec2<int> mousePos) {
    SDL_Point mousePoint{.x = mousePos.x, .y = mousePos.y};
    for (const auto& btn : buttonVector) {
        if (btn->isShown && SDL_PointInRect(&mousePoint, &btn->grid)) {
            if (btn->flags == SWITCH_BUTTON) {
                btn->isClicked = !btn->isClicked;
            }
            if (btn->clickEvent) {
                return btn->clickEvent();
            }
            break;  // Button handled, exit loop
        }
    }
    return Error();
}

void Button::HandleHover(Vec2<int> mousePos, SDL_Renderer* renderer) {
    SDL_Point mousePoint{.x = mousePos.x, .y = mousePos.y};
    for (const auto& btn : buttonVector) {
        if (btn->isShown && SDL_PointInRect(&mousePoint, &btn->grid)) {
            if (btn->hoverTexture) {
                scc(SDL_RenderCopy(renderer, btn->hoverTexture, nullptr, &btn->grid)).Handle();
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
