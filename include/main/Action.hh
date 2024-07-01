#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

#include <functional>
#include <unordered_map>

#include "../../lib/utils/math_utils.hh"
#include "../game/entities/Player.hh"

struct Key {
    SDL_Scancode key = SDL_SCANCODE_UNKNOWN;
    Uint8 mouseButton = 0;
    bool operator==(const Key& other) const {
        return key == other.key && mouseButton == other.mouseButton;
    }
};

namespace std {
template <>
struct hash<Key> {
    std::size_t operator()(const Key& k) const {
        return ((std::hash<int>()(static_cast<int>(k.key)) << 1) ^ std::hash<Uint8>()(k.mouseButton));
    }
};
}  // namespace std

struct Action {
   public:
    enum class ActionType {
        NOT_SET = -1,

        MOVE_UP = 0,
        MOVE_DOWN,
        MOVE_LEFT,
        MOVE_RIGHT,

        ATTACK1,
        ATTACK2,

        SWITCH_WEAPONS,
        OPEN_INVENTORY,

        QUIT_GAME,
    };

    Action();
    Action(ActionType actionType_);
    ~Action();

    ActionType GetActionType();

    void Activate();
    void Unactivate();

    bool IsActive();

    void SetIsHoldable(bool isHoldable_);
    bool IsHoldable();

    void SetFunction(std::function<void()> funtion_);

   private:
    ActionType actionType = Action::ActionType::NOT_SET;

    bool isActive = false;
    bool isHoldable = false;

    std::function<void()> function = nullptr;
};

class ActionHandler {
   public:
    void Handle();

    static ActionHandler* Instance(SDL_Event* event_, Player* playerInstance_, Vec2<int>* mousePos_, bool* shouldQuit_);

    void SetAction(Action action, Key key);

   private:
    ActionHandler(SDL_Event* event_, Player* playerInstance_, Vec2<int>* mousePos_, bool* shouldQuit_);
    ~ActionHandler();

    void HandleKeyboard();
    void HandleMouse();

    void Quit();

   private:
    static inline ActionHandler* instance;
    static inline std::unordered_map<Key, Action> keymap = {};

    SDL_Event* event = nullptr;

    Player* playerInstance = nullptr;

    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    Vec2<int>* mousePos = nullptr;
    Uint32 mouseState = SDL_GetMouseState(&mousePos->x, &mousePos->y);

    bool* shouldQuitGame = nullptr;
};
