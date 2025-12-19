#include "game_state.hpp"

GameStateManager::GameStateManager() : currentState(GameState::SPLASH), lastResult(GameResult::NONE) {}

void GameStateManager::setState(GameState state)
{
    currentState = state;
}

GameState GameStateManager::getState() const
{
    return currentState;
}

void GameStateManager::setResult(GameResult result)
{
    lastResult = result;
}

GameResult GameStateManager::getLastResult() const
{
    return lastResult;
}