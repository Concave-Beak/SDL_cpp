#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

#include <functional>
#include <unordered_map>

#include "../../lib/utils/math_utils.hh"
#include "../game/entities/Player.hh"

struct Key {
    SDL_KeyCode key = SDLK_UNKNOWN;
    Uint8 mouseButton = 0;
    bool operator==(const Key& other) const {
        return key == other.key && mouseButton == other.mouseButton;
    }
};

namespace std {
template <>
struct hash<Key> {
    std::size_t operator()(const Key& k) const {
        std::size_t h1 = std::hash<int>()(static_cast<int>(k.key));  // hash the key field
        std::size_t h2 = std::hash<Uint8>()(k.mouseButton);          // hash the mousebutton field

        return h1 ^ (h2 << 1);  // Combine the two hashes
    }
};
}  // namespace std

/*
 * Action struct | Where all of the actions related to the keyboard or mouse are handled.
 *
 * This engine does not support controller yet.
 */
struct Action {
   public:
    /* ActionType enum | list of all available actions.
     * Some ActionTypes are holdable eg MOVE_UP. These functions use the isActive field to keep active untill it's designated key is releassed.
     * Some ActionTypes also have a releasse function, this is done primarely for the ATTACK functions, so that when holding the button, the attack will be charged and releassing it releasses the attack.
     */
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

    void Activate() const;
    void Unactivate() const;

    bool IsActive();

    bool IsHoldable();

   private:
    void SetFunction(std::function<void()> funtion_);
    void SetReleaseFunction(std::function<void()> releaseFunction_);

    void SetIsHoldable(bool isHoldable_);

    ActionType actionType = Action::ActionType::NOT_SET;

    mutable bool isActive = false;
    bool isHoldable = false;

    std::function<void()> function = nullptr;

    std::function<void()> releaseFunction = nullptr;

    friend class ActionHandler;
};

class ActionHandler {
   public:
    void Handle();

    static ActionHandler* Instance(SDL_Event* event_, PlayerHandler* playerHandlerInstance_, Vec2<int>* mousePos_, bool* shouldQuit_);

    void SetAction(Action action, Key key);

   private:
    ActionHandler(SDL_Event* event_, PlayerHandler* playerHandlerInstance_, Vec2<int>* mousePos_, bool* shouldQuit_);
    ~ActionHandler();

    void HandleKeyboardPress(SDL_Keycode keycode);
    void HandleMousePress(Uint8 button);

    void HandleKeyboardRelease(SDL_Keycode keycode);
    void HandleMouseRelease(Uint8 button);

    void HandleActive();

    void Quit();

   private:
    static inline ActionHandler* instance;
    std::unordered_map<Key, Action> keymap = {};

    // function when button is pressed
    std::unordered_map<Action::ActionType, std::function<void()>> pressingFunctionMap;
    // function when button is releassed
    std::unordered_map<Action::ActionType, std::function<void()>> releassingFunctionMap;
    // check if is holdable when creating
    std::unordered_map<Action::ActionType, bool> isHoldableMap;

    SDL_Event* event = nullptr;

    PlayerHandler* playerHandlerInstance = nullptr;

    Vec2<int>* mousePos = nullptr;
    Uint32 mouseState = SDL_GetMouseState(&mousePos->x, &mousePos->y);

    bool* shouldQuitGame = nullptr;
};
