#ifndef __ENGINE_HEADER__
#define __ENGINE_HEADER__

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_ttf.h>
#include <sys/wait.h>

#include <string>
#include <unordered_map>

#include "../Entities/Player.hh"
#include "./Config.hh"

//------------------------------------------------------------------------------

class Camera {
   public:
    static Camera* GetCameraInstance();

    void FollowPlayer(Vec2f posPlayer, float delta, Vec2i cameraInfo, Vec2i hitboxPlayer);
    void Move(MoveOptions);

    std::array<Vec2f, 2> GetPlayerMinMaxOffset();
    void SetPlayerMinMaxOffsetMultiply(const float multiply);
    void SetPlayerMinMaxOffsetAbsolute(const float absVal);

   public:
    Vec2f pos = {0, 0};  // TODO: make private

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

class Engine {
   public:
    // SDL_Window* GetWindow();  // not implemented
    static Engine* GetEngineInstance();

    Vec2i GetScreenInfo();

    void Init();
    int Run();

    bool debugMode = false;  // will change
   private:
    static Engine* instance;

    Player* playerInstance = Player::GetPlayerInstace();
    Config* config = Config::GetConfig();
    Camera* camera = Camera::GetCameraInstance();

    SDL_Renderer* renderer = NULL;
    SDL_Window* window = NULL;
    SDL_Event event = SDL_Event{};

    std::unordered_map<SDL_Keycode, bool> keyStates;

    Uint16 fpsCap = 60;
    const Uint8 minFPS = 10;

    bool quit = false;
    bool paused = false;

    int SCREEN_WIDTH = 1024;
    int SCREEN_HEIGHT = 768;

    TTF_Font* debugFont;

   private:
    void Loop();

    void HandleFPS(float startTick);
    void HandleEvent(SDL_Event* event);
    void HandlePlayerVel(Vec2f* playerPos, Vec2f* playerVel, Vec2i playerHitboxInfo);

    void HandleColisions(Vec2f* playerPos, Vec2f* playerVel, Vec2i playerColisionboxInfo, float delta);

    void DrawText(const std::string& text, SDL_Rect textureRect, const SDL_Color fontColor);
    void ShowDebugInfo();
    int GetTextRectangleWidth(size_t strSize);

    void Render();
};

//------------------------------------------------------------------------------

#endif
