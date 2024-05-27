#pragma once

#include "../../lib/utils/engine_utils.hh"
#include "../../lib/utils/math_utils.hh"
#include "array"

//------------------------------------------------------------------------------

class Camera {
   public:
    static Camera* GetCameraInstance();
    Vector2<float> GetCameraPos();

    void FollowPlayer(Vector2<float> posPlayer, float delta, Vector2<int> cameraInfo,
                      Vector2<int> hitboxPlayer, float timeMultiplier, const bool& isPaused);
    void Move(Directions, const bool& isPaused);

    std::array<Vector2<float>, 2> GetPlayerMinMaxOffset();
    void SetPlayerMinMaxOffsetMultiply(const float multiply);
    void SetPlayerMinMaxOffsetAbsolute(const float absVal);

   public:
    static Vector2<float> pos;  // TODO: make private

   private:
    Vector2<float> vel = {0, 0};
    Vector2<float> playerOffset = {0, 0};

    Vector2<int> maxPlayerOffset = {10, 100};
    Vector2<float> minPlayerOffset = {-10, 100};  // both may change depending on player's items.
    Vector2<int> cameraMovementSpeed = {0, 0};
    // const int zoom = 1; // not used

    bool isBeingMoved = false;

   private:
    static Camera* instance;
};

//------------------------------------------------------------------------------
