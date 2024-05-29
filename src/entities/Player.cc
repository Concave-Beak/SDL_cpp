#include "../../include/entities/Player.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <cmath>

#include "../../lib/utils/math_utils.hh"

//------------------------------------------------------------------------------

Player* Player::playerInstance = new Player;

Vector2<float> Player::GetPos() { return entity.GetPos(); }
Vector2<float> Player::GetVelocity() { return entity.GetVelocity(); }
Player* Player::GetPlayerInstace() { return Player::playerInstance; }

Vector2<int> Player::GetHitbox() { return entity.GetHitbox(); }

//------------------------------------------------------------------------------
