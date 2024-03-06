#ifndef __PLAYER_HEADER__
#define __PLAYER_HEADER__

#include <SDL2/SDL.h>
#include "./EntityUtils.hh"

//------------------------------------------------------------------------------

enum MoveOpts {
    LEFT = 1,
    RIGHT,
    UP,
    DOWN,
};

class Player {
   public:
    Vec2f velocity = {10.0f, 10.0f};
    Vec2f pos = {0, 0};
    Uint32 lastUpdate;

   public:
    static Player* GetPlayerInstace();

    Vec2f* GetPlayerPos();
    void Move(const MoveOpts);
    void Draw(SDL_Renderer* renderer = NULL);

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
