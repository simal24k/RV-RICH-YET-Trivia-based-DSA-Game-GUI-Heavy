#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

enum class GameState
{
    SPLASH,
    MENU,
    PLAYER_SETUP,
    GAME_INTRO,
    QUESTION_DISPLAY,
    ANSWER_PROCESSING,
    RESULT_DISPLAY,
    PRIZE_LADDER, 
    GAME_OVER,
    FINAL_SCORE,
    LEADERBOARD,
    EXIT
};

enum class GameResult
{
    NONE,
    CORRECT,
    WRONG,
    QUIT
};

class GameStateManager
{
private:
    GameState currentState;
    GameResult lastResult;

public:
    GameStateManager();

    void setState(GameState state);
    GameState getState() const;
    void setResult(GameResult result);
    GameResult getLastResult() const;
};

#endif