#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include "game_engine.hpp"
#include "game_state.hpp"
#include "game_logic.hpp"
#include "timer.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class GameController
{
private:
    GameEngine &engine;
    GameStateManager stateManager;
    GameLogic gameLogic;
    GameTimer currentTimer;

    vector<int> hiddenOptions;
    string lifelineMessage;
    bool showLifelineMessage;

    void logStateChange(const string &msg) const;

public:
    GameController(GameEngine &gameEngine);

    void setState(GameState newState);
    void submitPlayerSetup(const std::string &name, const std::string &gender);
    void clearPause();
    void submitAnswer(int optionIndex);

    void update();

    void handleLifelineUsage(int lifelineType);
    void closeLifelineMessage();

    const vector<int> &getHiddenOptions() const;
    string getLifelineMessage() const;
    bool isLifelineMessageActive() const;

    void handleMenuInput();
    void handlePlayerSetup();
    void handleQuestionDisplay();
    void handleAnswerProcessing();
    void handleResultDisplay();
    void handleGameOver();
    void handleFinalScore();
    void handleLeaderboard();

    // NEW: Handler for Prize Ladder
    void handlePrizeLadder();

    GameState getState() const;
    bool isExiting() const;
};

#endif