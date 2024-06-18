#include "../../include/assetHandling/Font.hh"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

#include <cassert>
#include <cstdio>

#include "../../lib/utils/math_utils.hh"
#include "../../lib/utils/sdl_utils.hh"

//------------------------------------------------------------------------------

Font::Font(SDL_Renderer *renderer, const char *path) {
    SDL_Surface *fontSurface = SurfaceFromFile(path);
    scc(SDL_SetColorKey(fontSurface, SDL_TRUE, 0xff000000)).Handle();
    Result<void *> result = scp(SDL_CreateTextureFromSurface(renderer, fontSurface));
    spritesheet = (SDL_Texture *)result.result;
    SDL_FreeSurface(fontSurface);

    for (size_t ascii = ASCII_DISPLAY_LOW; ascii <= ASCII_DISPLAY_HIGH; ++ascii) {
        const size_t index = ascii - ASCII_DISPLAY_LOW;
        const size_t col = index % FONT_COLS;
        const size_t row = index / FONT_COLS;
        glyphTable[index] = SDL_Rect{
            .x = int(col * FONT_CHAR_WIDTH),
            .y = int(row * FONT_CHAR_HEIGHT),
            .w = FONT_CHAR_WIDTH,
            .h = FONT_CHAR_HEIGHT,
        };
    }
}

void Font::RenderChar(SDL_Renderer *renderer, char c, Vec2<int> pos, float scale) {
    const SDL_Rect dst = {
        .x = (int)floorf(float(pos.x)),
        .y = (int)floorf(float(pos.y)),
        .w = (int)floorf(FONT_CHAR_WIDTH * scale),
        .h = (int)floorf(FONT_CHAR_HEIGHT * scale),
    };

    assert(c >= ASCII_DISPLAY_LOW);
    assert(c <= ASCII_DISPLAY_HIGH);
    const size_t index = size_t(c - ASCII_DISPLAY_LOW);
    scc(SDL_RenderCopy(renderer, spritesheet, &glyphTable[index], &dst)).Handle();
}

void Font::RenderTextSized(SDL_Renderer *renderer, const char *text, size_t text_size, Vec2<int> pos,
                           SDL_Color color, float scale) {
    SetTextureColor(spritesheet, color);

    Vec2<int> pen = pos;
    for (size_t i = 0; i < text_size; ++i) {
        RenderChar(renderer, text[i], pen, scale);
        pen.x += int(FONT_CHAR_WIDTH * scale);
    }
}

void Font::RenderText(SDL_Renderer *renderer, const char *text, Vec2<int> pos, SDL_Color color,
                      float scale) {
    RenderTextSized(renderer, text, strlen(text), pos, color, scale);
}
