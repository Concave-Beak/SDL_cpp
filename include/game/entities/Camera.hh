#pragma once

#include <array>

#include "../../../lib/utils/engine_utils.hh"
#include "../../../lib/utils/math_utils.hh"

//------------------------------------------------------------------------------

class Camera {
   public:
    static Camera* GetCameraInstance();
    Vec2<float> GetCameraPos();

    void FollowPlayer(Vec2<float> posPlayer, float delta, Vec2<int> cameraInfo,
                      Vec2<int> hitboxPlayer, float timeMultiplier, const bool& isPaused);
    void Move(Direction, const bool& isPaused);

    std::array<Vec2<float>, 2> GetPlayerMinMaxOffset();
    void SetPlayerMinMaxOffsetMultiply(const float multiply);
    void SetPlayerMinMaxOffsetAbsolute(const float absVal);

   private:
    Vec2<float> pos;  // TODO: make private
    Vec2<float> vel = {0, 0};
    Vec2<float> playerOffset = {0, 0};

    Vec2<int> maxPlayerOffset = {10, 100};
    Vec2<float> minPlayerOffset = {-10, 100};  // both may change depending on player's items.
    Vec2<int> cameraMovementSpeed = {0, 0};
    // const int zoom = 1; // not used

    bool isBeingMoved = false;

   private:
    static Camera* instance;
};

//------------------------------------------------------------------------------
