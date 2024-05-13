#include <SDL2/SDL.h>

#include <array>

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
    SDL_Texture *spritesheet;
    std::array<SDL_Rect, ASCII_DISPLAY_HIGH - ASCII_DISPLAY_LOW + 1> glyphTable;
};

Font FontLoadFromFile(SDL_Renderer *renderer, const char *path);

void RenderTextSized(SDL_Renderer *renderer, Font *font, const char *text,
                     size_t text_size, Vector2<float> pos, SDL_Color color,
                     float scale);

void RenderText(SDL_Renderer *renderer, Font *font, const char *text,
                Vector2<float> pos, SDL_Color color, float scale);
