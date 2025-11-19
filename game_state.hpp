#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

using namespace std;

enum class GameState {
    MENU,
    PLAYER_SETUP,
    QUESTION_DISPLAY,
    ANSWER_PROCESSING,
    RESULT_DISPLAY,
    GAME_OVER,
    LEADERBOARD,
    EXIT
};

enum class GameResult {
    CORRECT,
    WRONG,
    QUIT
};

class GameStateManager {
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
