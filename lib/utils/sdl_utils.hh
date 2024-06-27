#pragma once

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

#include <cstdio>
#include <string>

#include "../../lib/stbi/stb_image.h"
#include "debug_utils.hh"
#include "math_utils.hh"

#define BLACK 0x00, 0x00, 0x00
#define PINK 0xec, 0x00, 0x8c
#define RED 0xff, 0x00, 0x00
#define YELLOW 0xff, 0xff, 0x00
#define WHITE 0xff, 0xff, 0xff
#define GREEN 0x00, 0xff, 0x00

#define UNHEX(color)                                                                       \
    ((color) >> (8 * 0)) & 0xFF, ((color) >> (8 * 1)) & 0xFF, ((color) >> (8 * 2)) & 0xFF, \
        ((color) >> (8 * 3)) & 0xFF

inline Error scc(int code) {
    if (code != 0) {
        return Error(Error::ErrorCode::SDL_FUNCTION_ERROR,
                     "SDL ERROR: " + std::string(SDL_GetError()),
                     Error::Severity::LOW);
    }
    return Error();
}

inline Result<void *> scp(void *ptr) {
    if (ptr == NULL) {
        return Result<void *>{
            .error = Error(Error::ErrorCode::SDL_FUNCTION_ERROR,
                           "SDL ERROR: %s" + std::string(SDL_GetError()),
                           Error::Severity::LOW),
            .result = nullptr,
        };
    }
    return Result<void *>{.error = Error(), .result = ptr};
}

inline SDL_Surface *SurfaceFromFile(std::string file_path) {
    int width, height, n;
    unsigned char *pixels = stbi_load(file_path.c_str(), &width, &height, &n, STBI_rgb_alpha);
    if (pixels == NULL) {
        fprintf(stderr, "ERROR: could not load file %s: %s\n", file_path.c_str(), stbi_failure_reason());
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

    Result<void *> surface = scp(SDL_CreateRGBSurfaceFrom((void *)pixels, width, height, depth, pitch, rmask, gmask, bmask, amask)).Handle();
    return (SDL_Surface *)surface.result;
}

inline void SetTextureColor(SDL_Texture *texture, SDL_Color color) {
    scc(SDL_SetTextureColorMod(texture, color.r, color.g, color.b)).Handle();

    scc(SDL_SetTextureAlphaMod(texture, color.a)).Handle();
}

// Draws that classic pink and black default texture in source games
//
// @param textureGrid The texture size to be drawn
// @param resolution The amout of pixels each color should be drawn
// @param renderer The renderer
inline void DrawTextureNotFound(SDL_Rect textureGrid, const Vec2<int> resolution, SDL_Renderer *renderer) {
    for (int y = 0; y < textureGrid.h / resolution.y; ++y) {
        for (int x = 0; x < textureGrid.w / resolution.x; ++x) {
            SDL_Rect rect{
                .x = textureGrid.x + x * resolution.x,
                .y = textureGrid.y + y * resolution.y,
                .w = resolution.x,
                .h = resolution.y,
            };
            SDL_Color color = ((x + y) % 2) ? SDL_Color{PINK, 0xff} : SDL_Color{BLACK, 0xff};

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

inline SDL_Rect operator+(const SDL_Rect &a, const SDL_Rect &b) {
    SDL_Rect result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.w = a.w + b.w;
    result.h = a.h + b.h;
    return result;
}

// Subtraction of two SDL_Rect
inline SDL_Rect operator-(const SDL_Rect &a, const SDL_Rect &b) {
    SDL_Rect result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.w = a.w - b.w;
    result.h = a.h - b.h;
    return result;
}
