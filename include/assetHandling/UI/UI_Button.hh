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
        NOT_SET = 0,
        PUSH_BUTTON,
        SWITCH_BUTTON,
    };

    enum TextureField {
        DEFAULT_TEXTURE = 0,
        HOVER_TEXTURE,
        CLICKED_TEXTURE,
    };

    Button(ButtonFlags type = NOT_SET, SDL_Rect grid = {});
    ~Button();

    void SetOutlineColor(const SDL_Color& color);
    Error SetTexture(SDL_Renderer* renderer, TextureField textureField, const std::string& path);
    Error SetTexture(SDL_Texture* texture, TextureField textureField);
    void SetFlags(ButtonFlags flags);

    static Error Handle(SDL_Event event, SDL_Renderer* renderer);

    void ToggleIsShown();
    void SetFunction(std::function<Error()> clickEvent);

   private:
    ButtonFlags flags = NOT_SET;
    SDL_Rect grid = {};
    SDL_Color outlineColor = {};
    SDL_Texture* defaultTexture = nullptr;
    SDL_Texture* hoverTexture = nullptr;
    SDL_Texture* clickedTexture = nullptr;

    bool isShown = true;
    bool isClicked = false;
    bool isHovered = false;

    std::function<Error()> clickEvent = nullptr;

    static std::vector<Button*> buttonVector;

   private:
    static Error DrawButtons(SDL_Renderer* renderer);
    static void HandleClicks(Vec2<int> mousePos);
    static void HandleHover(Vec2<int> mousePos, SDL_Renderer* renderer);

    inline static Vec2<int> lastMousepos = {};

    void AddToButtonVector();
    const Error AssignTexture(SDL_Texture*& oldTexture, SDL_Texture* newTexture);
};

}  // namespace UI
