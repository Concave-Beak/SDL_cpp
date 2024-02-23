#ifndef __PLAYER_HEADER__
#define __PLAYER_HEADER__

#include <SDL2/SDL.h>
#include "./EntityUtils.hh"

enum MoveOpts {
    LEFT = 1,
    RIGHT,
    UP,
    DOWN,
};

class Player {
   public:
    static float speed;

   public:
    static Player* GetPlayerInstace();
    static Position GetPlayerPos();

    void DefinePlayerSpeed(float);
    void Move(const MoveOpts);
    void Draw(SDL_Renderer* renderer);

   private:
    static Player* player;

    static Position new_pos;
    static Position prev_pos;
    static Position direction;

    const Hitbox hitbox = Hitbox{75, 75};

    void CalcPlayerSpeed();
};
#endif
