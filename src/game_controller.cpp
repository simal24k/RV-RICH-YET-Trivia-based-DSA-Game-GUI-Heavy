#include "game_controller.hpp"

using namespace std;

GameController::GameController(GameEngine &gameEngine)
    : engine(gameEngine), currentTimer(30), showLifelineMessage(false) {}

void GameController::logStateChange(const string &msg) const {}

void GameController::setState(GameState newState)
{
    stateManager.setState(newState);
}

void GameController::submitPlayerSetup(const std::string &name, const std::string &gender)
{
    if (stateManager.getState() == GameState::PLAYER_SETUP)
    {
        engine.setupPlayer(name, gender);
        stateManager.setState(GameState::GAME_INTRO);
    }
}

void GameController::submitAnswer(int optionIndex)
{
    if (showLifelineMessage)
        return;

    if (stateManager.getState() == GameState::ANSWER_PROCESSING ||
        stateManager.getState() == GameState::QUESTION_DISPLAY)
    {
        bool correct = engine.processAnswer(optionIndex);

        if (correct)
        {
            stateManager.setResult(GameResult::CORRECT);
            stateManager.setState(GameState::RESULT_DISPLAY);
        }
        else
        {
            stateManager.setResult(GameResult::WRONG);
            engine.endGame();
            stateManager.setState(GameState::RESULT_DISPLAY);
        }
    }
}

void GameController::clearPause()
{
    GameState currentState = stateManager.getState();

    if (currentState == GameState::RESULT_DISPLAY)
    {
        if (stateManager.getLastResult() == GameResult::CORRECT)
        {
            // FIX: Go to Prize Ladder on correct answer
            stateManager.setState(GameState::PRIZE_LADDER);
        }
        else
        {
            stateManager.setState(GameState::GAME_OVER);
        }
    }
    // NEW: Transition from Prize Ladder to Next Question or Win
    else if (currentState == GameState::PRIZE_LADDER)
    {
        if (engine.getPlayer().currentLevel >= 15)
        {
            engine.endGame(); // Save win
            stateManager.setState(GameState::GAME_OVER);
        }
        else
        {
            stateManager.setState(GameState::QUESTION_DISPLAY);
        }
    }
    else if (currentState == GameState::GAME_OVER)
    {
        stateManager.setState(GameState::FINAL_SCORE);
    }
    else if (currentState == GameState::FINAL_SCORE || currentState == GameState::LEADERBOARD)
    {
        stateManager.setState(GameState::MENU);
    }
}

void GameController::handleLifelineUsage(int lifelineType)
{
    if (showLifelineMessage)
        return;

    if (lifelineType == 0)
    {
        vector<int> removed = engine.use50_50Lifeline();
        if (!removed.empty())
        {
            hiddenOptions = removed;
        }
    }
    else if (lifelineType == 1)
    {
        int suggestion = engine.useAskFriendLifeline();
        if (suggestion != -1)
        {
            char opt = 'A' + suggestion;
            lifelineMessage = "Your friend thinks the answer is: " + string(1, opt);
            showLifelineMessage = true;
        }
    }
    else if (lifelineType == 2)
    {
        if (engine.useSkipLifeline())
        {
            handleQuestionDisplay();
        }
    }
    else if (lifelineType == 3)
    {
        string hint = engine.useHintLifeline();
        if (!hint.empty())
        {
            lifelineMessage = "HINT: " + hint;
            showLifelineMessage = true;
        }
    }
}

void GameController::closeLifelineMessage()
{
    showLifelineMessage = false;
}

const vector<int> &GameController::getHiddenOptions() const
{
    return hiddenOptions;
}

string GameController::getLifelineMessage() const
{
    return lifelineMessage;
}

bool GameController::isLifelineMessageActive() const
{
    return showLifelineMessage;
}

void GameController::handleMenuInput() {}
void GameController::handlePlayerSetup() {}

void GameController::handleQuestionDisplay()
{
    hiddenOptions.clear();
    showLifelineMessage = false;

    if (!engine.getNextQuestion())
    {
        engine.endGame();
        stateManager.setState(GameState::GAME_OVER);
        return;
    }

    int difficulty = gameLogic.getNextDifficulty(engine.getPlayer().currentLevel);
    int timeLimit = gameLogic.getTimeLimit(difficulty);

    currentTimer.setDuration(timeLimit);
    currentTimer.start();

    stateManager.setState(GameState::ANSWER_PROCESSING);
}

void GameController::handleAnswerProcessing()
{
    if (showLifelineMessage)
        return;

    currentTimer.update();

    if (currentTimer.isFinished())
    {
        stateManager.setResult(GameResult::WRONG);
        engine.endGame();
        stateManager.setState(GameState::GAME_OVER);
    }
}

void GameController::handleResultDisplay() {}
void GameController::handleGameOver() {}
void GameController::handleFinalScore() {}
void GameController::handleLeaderboard() {}

// NEW: Passive handler for Prize Ladder
void GameController::handlePrizeLadder() {}

void GameController::update()
{
    GameState currentState = stateManager.getState();

    switch (currentState)
    {
    case GameState::MENU:
        handleMenuInput();
        break;
    case GameState::PLAYER_SETUP:
        handlePlayerSetup();
        break;
    case GameState::QUESTION_DISPLAY:
        handleQuestionDisplay();
        break;
    case GameState::ANSWER_PROCESSING:
        handleAnswerProcessing();
        break;
    case GameState::RESULT_DISPLAY:
        handleResultDisplay();
        break;
    case GameState::PRIZE_LADDER:
        handlePrizeLadder();
        break; // NEW CASE
    case GameState::GAME_OVER:
        handleGameOver();
        break;
    case GameState::FINAL_SCORE:
        handleFinalScore();
        break;
    case GameState::LEADERBOARD:
        handleLeaderboard();
        break;
    case GameState::EXIT:
        break;
    case GameState::SPLASH:
        break;
    case GameState::GAME_INTRO:
        break;
    }
}

GameState GameController::getState() const { return stateManager.getState(); }
bool GameController::isExiting() const { return stateManager.getState() == GameState::EXIT; }