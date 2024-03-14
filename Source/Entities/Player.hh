#ifndef __PLAYER_HEADER__
#define __PLAYER_HEADER__

#include <SDL2/SDL.h>
#include "../../Include/Headers/Utils.hh"

//------------------------------------------------------------------------------

enum MoveOpts {
    LEFT = 1,
    RIGHT,
    UP,
    DOWN,
};

class Player {
   public:
    Vec2f velocity = {0, 0};
    Vec2f pos = {0, 0};
    Uint32 lastUpdate;

   public:
    static Player* GetPlayerInstace();

    Vec2f* GetPlayerPos();
    void Move(const MoveOpts);
    void Draw(SDL_Renderer* renderer = NULL);

    Vec2i GetHitboxInfo();

   private:
    static Player* player;

    Vec2i direction;

    Vec2i hitbox = {75, 75};
    SDL_Rect player_model = SDL_Rect{(int)pos.x, (int)pos.y, hitbox.x, hitbox.y};

   private:
    void CalcPlayerSpeed();
    void CalcPlayerAccel();
};

//------------------------------------------------------------------------------

#endif
