#include "../../../include/game/entities/EntityAttributes.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

EntityAttributes::EntityAttributes() { id = ++maxUsedID; }

Uint32 EntityAttributes::GetID() { return id; }

AttackAttributes::AttackAttributes() { id = ++maxUsedID; }

//------------------------------------------------------------------------------
