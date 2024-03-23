#ifndef __PLAYER_HEADER__
#define __PLAYER_HEADER__

#include <SDL2/SDL.h>

#include "../../Include/Headers/Utils.hh"

//------------------------------------------------------------------------------

class Player {
   public:
    Vec2f velocity = {0, 0};
    Vec2f pos = {0, 0};
    Uint32 lastUpdate;

    bool colidedDown = false;
    bool colidedLeft = false;
    bool colidedRight = false;
    bool colidedUp = false;

    bool isAbovePlatform = false;

    Vec2i hitbox = {75, 75};

   public:
    static Player* GetPlayerInstace();

    Vec2f* GetPlayerPos();
    void Move(const MoveOptions);

    Vec2i GetHitboxInfo();

   private:
    static Player* player;

    const Vec2f accelSpeed = {.x = 1, .y = 250};  // may change during gameplay,
                                                  // I'll just use it as a const it for
                                                  // now
};

//------------------------------------------------------------------------------

#endif
