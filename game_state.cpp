#include "game_state.hpp"

using namespace std;

GameStateManager::GameStateManager() : currentState(GameState::MENU), lastResult(GameResult::CORRECT) {}

void GameStateManager::setState(GameState state) {
    currentState = state;
}

GameState GameStateManager::getState() const {
    return currentState;
}

void GameStateManager::setResult(GameResult result) {
    lastResult = result;
}

GameResult GameStateManager::getLastResult() const {
    return lastResult;
}
