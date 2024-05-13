#include "../../../include/assetHandling/UI/UI_Button.hh"

#include <string>

#include "../../../lib/utils/sdl_utils.hh"

//------------------------------------------------------------------------------

UI::Button::Button(SDL_Rect grid_, std::string ID_) : grid(grid_) {
    if (ID_ == std::string{}) {
        ThrowError(LAWFULL_PARAMS, "Could not create button\n", Severity::MAJOR);
        return;
    }
    this->ID = ID_;

    this->textColor = SDL_Color{};
    this->outlineColor = SDL_Color{};
    this->fillColor = SDL_Color{};
    this->hoverColor = SDL_Color{};
}
UI::Button::~Button() {}

//------------------------------------------------------------------------------
