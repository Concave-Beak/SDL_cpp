#include "../../../include/game/entities/Camera.hh"

//------------------------------------------------------------------------------

Camera* Camera::instance = new Camera;

Camera* Camera::GetCameraInstance() { return instance; }

void Camera::FollowPlayer(Vec2<float> posPlayer, float delta, Vec2<int> cameraInfo,
                          Vec2<int> hitboxPlayer, float timeMultiplier, const bool& isPaused) {
    if (isPaused) return;
    playerOffset.x = posPlayer.x - pos.x - float(cameraInfo.x) / 2.0f + float(hitboxPlayer.x) / 2.0f;
    playerOffset.y = posPlayer.y - pos.y - float(cameraInfo.y) / 1.5f + float(hitboxPlayer.y) / 1.5f;

    vel.y *= (1 - delta) * timeMultiplier;  // case beingMoved
    vel.x *= (1 - delta) * timeMultiplier;
    if (!isBeingMoved) {
        vel.y = playerOffset.y * 0.1f * (1.0f - delta) * timeMultiplier;
        vel.x = playerOffset.x * 0.1f * (1.0f - delta) * timeMultiplier;
    }

    pos.y += vel.y;
    pos.x += vel.x;
    isBeingMoved = false;
}

void Camera::Move(Direction direction, const bool& isPaused) {
    if (isPaused) return;
    cameraMovementSpeed = {maxPlayerOffset.x / 2, maxPlayerOffset.y / 8};  // Values to make it smoother
    isBeingMoved = true;
    switch (direction) {
        case Direction::UP: {
            if (playerOffset.y < float(maxPlayerOffset.y)) {
                vel.y -= float(cameraMovementSpeed.y);
            }
            break;
        }
        case Direction::DOWN: {
            if (playerOffset.y < float(maxPlayerOffset.y)) {
                vel.y += float(cameraMovementSpeed.y);
            }
            break;
        }
        case Direction::LEFT: {
            if (playerOffset.x > float(minPlayerOffset.x)) {
                vel.x -= float(cameraMovementSpeed.x);
            }
            break;
        }
        case Direction::RIGHT: {
            if (playerOffset.x < float(maxPlayerOffset.x)) {
                vel.x += float(cameraMovementSpeed.x);
            }
            break;
        }
    }
}

Vec2<float> Camera::GetCameraPos() { return pos; }
