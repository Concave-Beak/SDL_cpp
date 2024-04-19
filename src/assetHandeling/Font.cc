#include "../../include/assetHandeling/Font.hh"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

#include <cstdio>
#include <cassert>

#include "../../lib/utils/sdl_utils.hh"

Font FontLoadFromFile(SDL_Renderer *renderer, std::string path) {
    Font font = Font{};

    SDL_Surface *fontSurface = SurfaceFromFile(path);
    scc(SDL_SetColorKey(fontSurface, SDL_TRUE, 0xff000000));
    font.spritesheet = (SDL_Texture *)scp(SDL_CreateTextureFromSurface(renderer, fontSurface));
    SDL_FreeSurface(fontSurface);

    for (size_t ascii = ASCII_DISPLAY_LOW; ascii <= ASCII_DISPLAY_HIGH; ++ascii) {
        const int index = ascii - ASCII_DISPLAY_LOW;
        const int col = index % FONT_COLS;
        const int row = index / FONT_COLS;
        font.glyphTable[index] = SDL_Rect{
            .x = col * FONT_CHAR_WIDTH,
            .y = row * FONT_CHAR_HEIGHT,
            .w = FONT_CHAR_WIDTH,
            .h = FONT_CHAR_HEIGHT,
        };
    }
    return font;
}

void RenderTextSized(SDL_Renderer *renderer, Font *font, const char *text, size_t text_size, Vec2f pos, Uint32 color, float scale) {
    SetTextureColor(font->spritesheet, color);

    Vec2f pen = pos;
    for (size_t i = 0; i < text_size; ++i) {
        RenderChar(renderer, font, text[i], pen, scale);
        pen.x += FONT_CHAR_WIDTH * scale;
    }
}

void RenderChar(SDL_Renderer *renderer, const Font *font, char c, Vec2f pos, float scale) {
    const SDL_Rect dst = {
        .x = (int)floorf(pos.x),
        .y = (int)floorf(pos.y),
        .w = (int)floorf(FONT_CHAR_WIDTH * scale),
        .h = (int)floorf(FONT_CHAR_HEIGHT * scale),
    };

    assert(c >= ASCII_DISPLAY_LOW);
    assert(c <= ASCII_DISPLAY_HIGH);
    const size_t index = c - ASCII_DISPLAY_LOW;
    scc(SDL_RenderCopy(renderer, font->spritesheet, &font->glyphTable[index], &dst));
}
