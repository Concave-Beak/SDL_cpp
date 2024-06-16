#include "../..//include/main/Action.hh"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <utility>

Action::Action() : actionType(Action::ActionType::NOT_SET) {}
Action::Action(Action::ActionType type_) : actionType(type_) {}
Action::~Action() {}

Action::ActionType Action::GetActionType() { return actionType; }

void Action::Activate() {
    if (!isHoldable && isActive) return;
    this->isActive = true;
    this->function();
}

void Action::Unactivate() { this->isActive = false; }

bool Action::IsActive() { return isActive; }

bool Action::IsHoldable() { return isHoldable; }
void Action::SetIsHoldable(bool isHoldable_) { isHoldable = isHoldable_; }

void Action::SetFunction(std::function<void()> function_) { function = function_; }

ActionHandler::ActionHandler(SDL_Event* event_, Player* playerInstance_, Vec2<int>* mousePos_, bool* shouldQuit_) : event(event_), playerInstance(playerInstance_), mousePos(mousePos_), shouldQuitGame(shouldQuit_) {}

ActionHandler* ActionHandler::GetActionHandler(SDL_Event* event_, Player* playerInstance_, Vec2<int>* mousePos_, bool* shouldQuit_) {
    if (instance != nullptr) {
        return instance;
    }
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
        }
    }
    HandleKeyboard();
    HandleMouse();
}

void ActionHandler::SetAction(Action action, Key key) {
    switch (action.GetActionType()) {
        case Action::ActionType::NOT_SET: {
            std::cerr << "Action not set\n";
            return;
        }
        case Action::ActionType::MOVE_UP: {
            action.SetFunction(std::bind(
                &Player::Move, playerInstance, Direction::UP, playerInstance->GetRunningSpeed(), false));
            action.SetIsHoldable(true);
            break;
        }
        case Action::ActionType::MOVE_DOWN: {
            action.SetFunction(std::bind(
                &Player::Move, playerInstance, Direction::DOWN, playerInstance->GetRunningSpeed(), false));
            action.SetIsHoldable(true);
            break;
        }
        case Action::ActionType::MOVE_LEFT: {
            action.SetFunction(std::bind(
                &Player::Move, playerInstance, Direction::LEFT, playerInstance->GetRunningSpeed(), false));
            action.SetIsHoldable(true);
            break;
        }
        case Action::ActionType::MOVE_RIGHT: {
            action.SetFunction(std::bind(
                &Player::Move, playerInstance, Direction::RIGHT, playerInstance->GetRunningSpeed(), false));
            action.SetIsHoldable(true);
            break;
        }
        case Action::ActionType::ATTACK1: {
            action.SetFunction(std::bind(&Player::Attack, playerInstance));
            break;
        }
        case Action::ActionType::ATTACK2: {
            action.SetFunction(std::bind(&Player::Attack, playerInstance));
            action.SetIsHoldable(true);
            break;
        }
        case Action::ActionType::SWITCH_WEAPONS: {  // TODO:PLACEHOLDEr
            action.SetFunction(std::bind(
                &Player::Move, playerInstance, Direction::RIGHT, Vec2<float>{0, 0}, false));
            break;
        }
        case Action::ActionType::OPEN_INVENTORY: {
            action.SetFunction(std::bind(
                &Player::Move, playerInstance, Direction::RIGHT, Vec2<float>{0, 0}, false));
            break;
        }
        case Action::ActionType::QUIT_GAME: {
            action.SetFunction(std::bind(&ActionHandler::Quit, this));
            break;
        }
    }
    keymap.insert(std::make_pair(key, action));
}

void ActionHandler::HandleKeyboard() {
    for (std::unordered_map<Key, Action>::iterator it = keymap.begin(); it != keymap.end(); ++it) {
        if (it->first.key == SDL_SCANCODE_UNKNOWN) continue;

        if (keyboardState[it->first.key]) {
            it->second.Activate();
        } else {
            it->second.Unactivate();
        }
    }
}

void ActionHandler::HandleMouse() {
    mouseState = SDL_GetMouseState(&mousePos->x, &mousePos->y);
    for (std::unordered_map<Key, Action>::iterator it = keymap.begin(); it != keymap.end(); ++it) {
        if (it->first.mouseButton == 0) continue;

        if (mouseState & SDL_BUTTON(it->first.mouseButton)) {
            it->second.Activate();
        } else {
            it->second.Unactivate();
        }
    }
}

void ActionHandler::Quit() { *shouldQuitGame = true; }
