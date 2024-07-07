#include "../../include/main/Action.hh"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <utility>

#include "../../include/game/entities/Player.hh"

Action::Action() : actionType(Action::ActionType::NOT_SET) {}
Action::Action(Action::ActionType type_) : actionType(type_) {}
Action::~Action() {}

Action::ActionType Action::GetActionType() { return actionType; }

void Action::Activate() const {
    if (!isHoldable && isActive) return;
    this->isActive = true;
    this->function();
}

void Action::Unactivate() const {
    isActive = false;
    if (releaseFunction != nullptr) releaseFunction();
}

bool Action::IsActive() { return isActive; }

bool Action::IsHoldable() { return isHoldable; }
void Action::SetIsHoldable(bool isHoldable_) { isHoldable = isHoldable_; }

void Action::SetFunction(std::function<void()> function_) { function = function_; }
void Action::SetReleaseFunction(std::function<void()> function_) { releaseFunction = function_; }

//------------------------------------------------------------------------------

ActionHandler::ActionHandler(SDL_Event* event_, Player* playerInstance_, Vec2<int>* mousePos_, bool* shouldQuit_) : event(event_), playerInstance(playerInstance_), mousePos(mousePos_), shouldQuitGame(shouldQuit_) {}

ActionHandler* ActionHandler::Instance(SDL_Event* event_, Player* playerInstance_, Vec2<int>* mousePos_, bool* shouldQuit_) {
    instance = new ActionHandler(event_, playerInstance_, mousePos_, shouldQuit_);
    return instance;
}

void ActionHandler::Handle() {
    while (SDL_PollEvent(event)) {
        switch (event->type) {
            case SDL_QUIT: {
                *shouldQuitGame = true;
                break;
            }
            case SDL_KEYDOWN: {
                HandleKeyboardPress(event->key.keysym.sym);
                break;
            }
            case SDL_KEYUP: {
                HandleKeyboardRelease(event->key.keysym.sym);
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                HandleMousePress(event->button.button);
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                HandleMouseRelease(event->button.button);
                break;
            }
        }
    }

    mouseState = SDL_GetMouseState(&mousePos->x, &mousePos->y);
    HandleActive();
}

void ActionHandler::HandleActive() {
    for (std::unordered_map<Key, Action>::iterator it = keymap.begin(); it != keymap.end(); ++it) {
        if (it->second.IsActive()) {
            it->second.Activate();
        }
    }
}

void ActionHandler::SetAction(Action action, Key key) {
    std::function<void()> actionFunc = nullptr;
    std::function<void()> releaseFunc = nullptr;
    bool isHoldable_ = false;
    switch (action.GetActionType()) {
        case Action::ActionType::NOT_SET: {
            actionFunc = []() -> void { std::cerr << "Mapping not Set\n"; };
            break;
        }
        case Action::ActionType::MOVE_UP: {
            actionFunc = std::bind(&Player::Move, playerInstance, Direction::UP, playerInstance->GetRunningSpeed(), false);
            isHoldable_ = true;
            break;
        }
        case Action::ActionType::MOVE_DOWN: {
            actionFunc = std::bind(&Player::Move, playerInstance, Direction::DOWN, playerInstance->GetRunningSpeed(), false);
            isHoldable_ = true;
            break;
        }
        case Action::ActionType::MOVE_LEFT: {
            actionFunc = std::bind(&Player::Move, playerInstance, Direction::LEFT, playerInstance->GetRunningSpeed(), false);
            isHoldable_ = true;
            break;
        }
        case Action::ActionType::MOVE_RIGHT: {
            actionFunc = std::bind(&Player::Move, playerInstance, Direction::RIGHT, playerInstance->GetRunningSpeed(), false);
            isHoldable_ = true;
            break;
        }
        case Action::ActionType::ATTACK1: {
            actionFunc = std::bind(&Player::ChargeAttack, playerInstance);
            releaseFunc = std::bind(&Player::ReleaseAttack, playerInstance);
            isHoldable_ = true;
            break;
        }
        case Action::ActionType::ATTACK2: {
            actionFunc = std::bind(&Player::ReleaseAttack, playerInstance);
            releaseFunc = std::bind(&Player::ReleaseAttack, playerInstance);
            isHoldable_ = true;
            break;
        }
        case Action::ActionType::SWITCH_WEAPONS: {
            actionFunc = []() -> void { std::cerr << "Mapping not Set\n"; };
            break;
        }
        case Action::ActionType::OPEN_INVENTORY: {
            actionFunc = []() -> void { std::cerr << "Mapping not Set\n"; };
            break;
        }
        case Action::ActionType::QUIT_GAME: {
            actionFunc = std::bind(&ActionHandler::Quit, this);
            break;
        }
    }
    action.SetFunction(actionFunc);  // should not crash lol
    action.SetIsHoldable(isHoldable_);
    action.SetReleaseFunction(releaseFunc);

    keymap.insert(std::make_pair(key, action));
}

void ActionHandler::HandleKeyboardPress(SDL_Keycode keycode) {
    for (std::unordered_map<Key, Action>::iterator it = keymap.begin(); it != keymap.end(); ++it) {
        if (it->first.key == keycode) it->second.Activate();
    }
}

void ActionHandler::HandleKeyboardRelease(SDL_Keycode keycode) {
    for (std::unordered_map<Key, Action>::iterator it = keymap.begin(); it != keymap.end(); ++it) {
        if (it->first.key == SDLK_UNKNOWN) continue;
        if (it->first.key == keycode) {
            it->second.Unactivate();
        }
    }
}

void ActionHandler::HandleMousePress(Uint8 button) {
    for (std::unordered_map<Key, Action>::iterator it = keymap.begin(); it != keymap.end(); ++it) {
        if (it->first.mouseButton == button) it->second.Activate();
    }
}

void ActionHandler::HandleMouseRelease(Uint8 button) {
    for (std::unordered_map<Key, Action>::iterator it = keymap.begin(); it != keymap.end(); ++it) {
        if (it->first.mouseButton == button) it->second.Unactivate();
    }
}

void ActionHandler::Quit() { *shouldQuitGame = true; }
