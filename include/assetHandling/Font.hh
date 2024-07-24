#pragma once

#include <SDL2/SDL.h>

#include <array>
#include <cstdio>

#include "../../lib/utils/math_utils.hh"

#define ASCII_DISPLAY_LOW 32
#define ASCII_DISPLAY_HIGH 126

#define FONT_WIDTH 128
#define FONT_HEIGHT 64
#define FONT_COLS 18
#define FONT_ROWS 7
#define FONT_CHAR_WIDTH (int)((float)FONT_WIDTH / FONT_COLS)
#define FONT_CHAR_HEIGHT (int)((float)FONT_HEIGHT / FONT_ROWS)

// heavely inspired by tsoding's text editor `ded`
struct Font {
   public:
    bool IsEmpty() const { return spritesheet == nullptr; }

    Font() = default;
    ~Font() = default;
    Font(SDL_Renderer *renderer, const char *path);

   public:
    SDL_Texture *spritesheet = nullptr;
    std::array<SDL_Rect, ASCII_DISPLAY_HIGH - ASCII_DISPLAY_LOW + 1> glyphTable = {};

   public:
    void RenderTextSized(SDL_Renderer *renderer, const char *text, size_t text_size, Vec2<int> pos,
                         SDL_Color color, float scale);

    void RenderText(SDL_Renderer *renderer, const char *text, Vec2<int> pos, SDL_Color color,
                    float scale);

   private:
    void RenderChar(SDL_Renderer *renderer, char c, Vec2<int> pos, float scale);
};
