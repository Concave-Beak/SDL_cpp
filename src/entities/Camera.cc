#include "../../include/entities/Camera.hh"

//------------------------------------------------------------------------------

Camera *Camera::instance = new Camera;

Camera *Camera::GetCameraInstance() { return instance; }

Vector2<float> Camera::pos = {0, 0};

void Camera::FollowPlayer(Vector2<float> posPlayer, float delta, Vector2<int> cameraInfo,
                      Vector2<int> hitboxPlayer, float timeMultiplier, const bool& isPaused) {
    playerOffset.x = posPlayer.x - pos.x - cameraInfo.x / 2.0f + hitboxPlayer.x / 2.0f;
    playerOffset.y = posPlayer.y - pos.y - cameraInfo.y / 1.5f + hitboxPlayer.y / 1.5f;

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

void Camera::Move(MoveOptions moveOpt, const bool& isPaused) {
    cameraMovementSpeed = {maxPlayerOffset.x / 2, maxPlayerOffset.y / 8};  // Values to make it smoother
    isBeingMoved = true;
    switch (moveOpt) {
        case NONE: {
            break;
        }
        case UP: {
            if (playerOffset.y < maxPlayerOffset.y) {
                vel.y -= cameraMovementSpeed.y;
            }
            break;
        }
        case DOWN: {
            if (playerOffset.y < maxPlayerOffset.y) {
                vel.y += cameraMovementSpeed.y;
            }
            break;
        }
        case LEFT: {
            if (playerOffset.x > minPlayerOffset.x) {
                vel.x -= cameraMovementSpeed.x;
            }
            break;
        }
        case RIGHT: {
            if (playerOffset.x < maxPlayerOffset.x) {
                vel.x += cameraMovementSpeed.x;
            }
            break;
        }
    }
}

Vector2<float> Camera::GetCameraPos() { return pos; }
