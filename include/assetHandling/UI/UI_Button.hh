#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>

#include <functional>
#include <string>
#include <vector>

#include "../../../lib/utils/error_handling.hh"
#include "../../../lib/utils/math_utils.hh"

namespace UI {

class Button {
   public:
    enum ButtonFlags {
        SWITCH_BUTTON = 0x01,
    };

    Button(ButtonFlags type_, SDL_Rect grid_);
    ~Button();

    void SetOutlineColor(SDL_Color& color);
    enum TextureField {
        DEFAULT_TEXTURE = 0,
        HOVER_TEXTURE,
        CLICKED_TEXTURE,
    };
    const Error SetTexture(SDL_Renderer* renderer, const TextureField& textureField, std::string path);
    void SetFlags(int flags_);

    static const Error DrawButtons(SDL_Renderer* renderer);

    static void HandleButtonClicks(Vector2<int>);

    void ToggleIsShown();
    inline void SetFunction(std::function<Error()> clickEvent_) { clickEvent = clickEvent_; };

   private:
    static const Error DrawTextureBtn(SDL_Renderer* renderer, const Button* btn);

   private:
    Uint8 flags;

    SDL_Rect grid = {};

    SDL_Color outlineColor = {};

    SDL_Texture* defaultTexture = nullptr;
    SDL_Texture* hoverTexture = nullptr;
    SDL_Texture* clickedTexture = nullptr;

    bool isShown = true;

    bool isClicked = false;
    bool isHovered = false;

    std::function<Error()> clickEvent;
};

inline static std::vector<Button*> buttonVector{};

}  // namespace UI
