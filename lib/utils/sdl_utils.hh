#pragma once

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

#include <cstdio>
#include <string>

#include "../../lib/stbi/stb_image.h"
#include "math_utils.hh"

#define BLACK \
    SDL_Color { 0x00, 0x00, 0x00, 0xff }
#define PINK \
    SDL_Color { 0xec, 0x00, 0x8c, 0xff }
#define RED \
    SDL_Color { 0xff, 0x00, 0x00, 0xff }
#define WHITE \
    SDL_Color { 0xff, 0xff, 0xff, 0xff }
#define GREEN \
    SDL_Color { 0x00, 0xff, 0x00, 0xff }

#define UNHEX(color)                 \
    ((color) >> (8 * 0)) & 0xFF,     \
        ((color) >> (8 * 1)) & 0xFF, \
        ((color) >> (8 * 2)) & 0xFF, \
        ((color) >> (8 * 3)) & 0xFF

inline void scc(int code) {
    if (code != 0) {
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
        exit(1);
    }
}

inline void *scp(void *ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
        exit(1);
    }
    return ptr;
}

inline SDL_Surface *SurfaceFromFile(std::string file_path) {
    int width, height, n;
    unsigned char *pixels = stbi_load(file_path.c_str(), &width, &height, &n, STBI_rgb_alpha);
    if (pixels == NULL) {
        fprintf(stderr, "ERROR: could not load file %s: %s\n",
                file_path.c_str(), stbi_failure_reason());
        exit(1);  // TODO: create a default surface for unloaded textures
    }

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    const Uint32 rmask = 0xff000000;
    const Uint32 gmask = 0x00ff0000;
    const Uint32 bmask = 0x0000ff00;
    const Uint32 amask = 0x000000ff;
#else
    const Uint32 rmask = 0x000000ff;
    const Uint32 gmask = 0x0000ff00;
    const Uint32 bmask = 0x00ff0000;
    const Uint32 amask = 0xff000000;
#endif

    const int depth = 32;
    const int pitch = 4 * width;

    return (SDL_Surface *)scp(SDL_CreateRGBSurfaceFrom(
        (void *)pixels, width, height, depth, pitch,
        rmask, gmask, bmask, amask));
}

inline void SetTextureColor(SDL_Texture *texture, SDL_Color color) {
    scc(SDL_SetTextureColorMod(
        texture,
        color.r,
        color.g,
        color.b));

    scc(SDL_SetTextureAlphaMod(texture, color.a));
}

// Draws that classic pink and black default texture in source games
//
// @param textureGrid The texture size to be drawn
// @param resolution The amout of pixels each color should be drawn
// @param renderer The renderer
inline void DrawTextureNotFound(SDL_Rect textureGrid, const Vector2<int> resolution, SDL_Renderer *renderer) {
    for (int y = 0; y < textureGrid.h / resolution.y; ++y) {
        for (int x = 0; x < textureGrid.w / resolution.x; ++x) {
            SDL_Rect rect{
                .x = textureGrid.x + x * resolution.x,
                .y = textureGrid.y + y * resolution.y,
                .w = resolution.x,
                .h = resolution.y,
            };
            SDL_Color color = ((x + y) % 2) ? PINK : BLACK;

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}
