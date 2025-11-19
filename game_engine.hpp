#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include "data_structures.hpp"
#include "question_bank.hpp"
#include "leaderboard.hpp"
#include "game_logic.hpp"
#include "timer.hpp"
#include "player_profile.hpp"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

class QuoteManager {
private:
    vector<string> quotes;
    
public:
    QuoteManager();
    string getRandomQuote() const;
    void displayRandomQuote() const;
};

class GameEngine {
private:
    Player player;
    QuestionBank questionBank;
    PrizeLadder prizeLadder;
    LifelineStack lifelineStack;
    Leaderboard leaderboard;
    GameLogic gameLogic;
    GameTimer gameTimer;
    QuoteManager quoteManager; // Quote manager instance
    Question currentQuestion;
    int timeLimit;
    bool gameActive;
    int correctAnswerStreak;
    int totalPointsEarned;
    PlayerProfileManager playerProfileManager;
    bool showGameStatus; // Control when to show status

public:
    GameEngine();

    bool initialize(const string& questionsFile);
    void setupPlayer(const string& name, const string& gender);
    bool getNextQuestion();
    bool processAnswer(int optionIndex);
    vector<int> use50_50Lifeline();
    int useAskFriendLifeline();
    bool useSkipLifeline();
    string useHintLifeline(); // New hint lifeline method
    bool isLifelineAvailable(int lifelineType) const;
    void endGame();
    void displayGameStatus();
    void displayQuestion();
    void displayStartStatus(); // Status at game start
    void displayEndStatus(); // Status at game end
    
    const Player& getPlayer() const;
    const PrizeLadder& getPrizeLadder() const;
    const Question& getCurrentQuestion() const;
    Leaderboard& getLeaderboard();
    int getTotalPoints() const;
    int getCorrectStreak() const;
    PlayerStats getPlayerProfile() ;
    PlayerProfileManager& getProfileManager();
};

#endif
