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
    static Vec2f speed;

   public:
    static Player* GetPlayerInstace();
    static Vec2i GetPlayerPos();

    void DefinePlayerSpeed(float, float);
    void Move(const MoveOpts);
    void Draw(SDL_Renderer* renderer);

   private:
    static Player* player;

    static Vec2i pos;
    static Vec2i direction;

    static Vec2i hitbox;
    static SDL_Rect player_model;

   private:
    void CalcPlayerSpeed();
    void CalcPlayerAccel();
};
#endif
