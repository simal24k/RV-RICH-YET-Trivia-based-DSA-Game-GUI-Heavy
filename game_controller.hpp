#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include "game_engine.hpp"
#include "game_state.hpp"
#include "game_logic.hpp"
#include "timer.hpp"
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>

using namespace std;

class GameController {
private:
    GameEngine& engine;
    GameStateManager stateManager;
    GameLogic gameLogic;
    GameTimer currentTimer;
    string inputBuffer;
    bool waitingForInput;

public:
    GameController(GameEngine& gameEngine);

    void handleMenuInput();
    void handlePlayerSetup();
    void handleQuestionDisplay();
    void handleAnswerProcessing();
    void handleResultDisplay();
    void handleGameOver();
    void handleLeaderboard();
    void handleLifelineUsage(int lifelineType);
    void update();
    GameState getState() const;
    bool isExiting() const;
};

#endif
