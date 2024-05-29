#include "../../include/assetHandling/Font.hh"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

#include <cassert>
#include <cstdio>

#include "../../lib/utils/sdl_utils.hh"

//------------------------------------------------------------------------------

Font FontLoadFromFile(SDL_Renderer *renderer, const char *path) {
    Font font = Font{};
    SDL_Surface *fontSurface = SurfaceFromFile(path);
    scc(SDL_SetColorKey(fontSurface, SDL_TRUE, 0xff000000));
    font.spritesheet = (SDL_Texture *)scp(SDL_CreateTextureFromSurface(renderer, fontSurface));
    SDL_FreeSurface(fontSurface);

    for (size_t ascii = ASCII_DISPLAY_LOW; ascii <= ASCII_DISPLAY_HIGH; ++ascii) {
        const size_t index = ascii - ASCII_DISPLAY_LOW;
        const size_t col = index % FONT_COLS;
        const size_t row = index / FONT_COLS;
        font.glyphTable[index] = SDL_Rect{
            .x = int(col * FONT_CHAR_WIDTH),
            .y = int(row * FONT_CHAR_HEIGHT),
            .w = FONT_CHAR_WIDTH,
            .h = FONT_CHAR_HEIGHT,
        };
    }
    return font;
}

void RenderChar(SDL_Renderer *renderer, const Font *font, char c, Vector2<int> pos, float scale) {
    const SDL_Rect dst = {
        .x = (int)floorf(float(pos.x)),
        .y = (int)floorf(float(pos.y)),
        .w = (int)floorf(FONT_CHAR_WIDTH * scale),
        .h = (int)floorf(FONT_CHAR_HEIGHT * scale),
    };

    assert(c >= ASCII_DISPLAY_LOW);
    assert(c <= ASCII_DISPLAY_HIGH);
    const size_t index = size_t(c - ASCII_DISPLAY_LOW);
    scc(SDL_RenderCopy(renderer, font->spritesheet, &font->glyphTable[index], &dst));
}

void RenderTextSized(SDL_Renderer *renderer, Font *font, const char *text, size_t text_size, Vector2<int> pos,
                     SDL_Color color, float scale) {
    SetTextureColor(font->spritesheet, color);

    Vector2<int> pen = pos;
    for (size_t i = 0; i < text_size; ++i) {
        RenderChar(renderer, font, text[i], pen, scale);
        pen.x += int(FONT_CHAR_WIDTH * scale);
    }
}

void RenderText(SDL_Renderer *renderer, Font *font, const char *text, Vector2<int> pos, SDL_Color color,
                float scale) {
    RenderTextSized(renderer, font, text, strlen(text), pos, color, scale);
}
