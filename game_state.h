#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <string>
#include <iostream>
using namespace std;
enum class GameState {
    MENU,
    PLAYER_SETUP,
    QUESTION_DISPLAY,
    LIFELINE_SELECTION,
    ANSWER_PROCESSING,
    RESULT_DISPLAY,
    GAME_OVER,
    LEADERBOARD,
    EXIT
};

enum class GameResult {
    CORRECT,
    WRONG,
    TIMEOUT,
    QUIT
};

struct StateTransition {
    GameState fromState;
    GameState toState;
     string action;

    StateTransition(GameState from, GameState to, const  string& act)
        : fromState(from), toState(to), action(act) {}
};

class GameStateManager {
private:
    GameState currentState;
    GameState previousState;
    GameResult lastResult;
    bool stateChanged;

public:
    GameStateManager() 
        : currentState(GameState::MENU), 
          previousState(GameState::MENU),
          lastResult(GameResult::QUIT),
          stateChanged(true) {}

    void setState(GameState newState) {
        if (newState != currentState) {
            previousState = currentState;
            currentState = newState;
            stateChanged = true;
        }
    }

    GameState getState() const { return currentState; }
    GameState getPreviousState() const { return previousState; }
    bool hasStateChanged() const { return stateChanged; }
    
    void acknowledgeStateChange() { stateChanged = false; }

    void setResult(GameResult result) { lastResult = result; }
    GameResult getLastResult() const { return lastResult; }

     string getStateString() const {
        switch (currentState) {
            case GameState::MENU: return "MAIN_MENU";
            case GameState::PLAYER_SETUP: return "PLAYER_SETUP";
            case GameState::QUESTION_DISPLAY: return "QUESTION_DISPLAY";
            case GameState::LIFELINE_SELECTION: return "LIFELINE_SELECTION";
            case GameState::ANSWER_PROCESSING: return "ANSWER_PROCESSING";
            case GameState::RESULT_DISPLAY: return "RESULT_DISPLAY";
            case GameState::GAME_OVER: return "GAME_OVER";
            case GameState::LEADERBOARD: return "LEADERBOARD";
            case GameState::EXIT: return "EXIT";
            default: return "UNKNOWN";
        }
    }

    void displayStateTransition() {
         cout << "[STATE] " << getStateString() << " -> ";
    }
};

#endif
