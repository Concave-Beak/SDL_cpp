#ifndef __CAMERA_HEADER__
#define __CAMERA_HEADER__
#include "../../Include/Headers/Utils.hh"
#include "array"

//------------------------------------------------------------------------------

class Camera {
   public:
    static Camera* GetCameraInstance();

    void FollowPlayer(Vec2f posPlayer, float delta, Vec2i cameraInfo, Vec2i hitboxPlayer, float timeMultiplier);
    void Move(MoveOptions);

    std::array<Vec2f, 2> GetPlayerMinMaxOffset();
    void SetPlayerMinMaxOffsetMultiply(const float multiply);
    void SetPlayerMinMaxOffsetAbsolute(const float absVal);

   public:
    static Vec2f pos;  // TODO: make private

   private:
    Vec2f vel = {0, 0};
    Vec2f playerOffset = {0, 0};

    Vec2i maxPlayerOffset = {10, 100};
    Vec2f minPlayerOffset = {-10, 100};  // both may change depending on player's items.
    Vec2i cameraMovementSpeed = {0, 0};
    // const int zoom = 1; // not used

    bool isBeingMoved = false;

   private:
    static Camera* instance;
};

//------------------------------------------------------------------------------
#endif
