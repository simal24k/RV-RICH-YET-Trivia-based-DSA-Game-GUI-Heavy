#ifndef GAME_LOGIC_HPP
#define GAME_LOGIC_HPP

#include "data_structures.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

class GameLogic {
private:
    static constexpr int TOTAL_LIFELINES = 4; // Updated to 4 lifelines (added Hint)
    
    enum LifelineType {
        FIFTY_FIFTY = 0,
        ASK_FRIEND = 1,
        SKIP = 2,
        HINT = 3 // New hint lifeline
    };

    struct LifelineState {
        bool used;
        int usageCount;
        
        LifelineState();
    };

    LifelineState lifelineStates[TOTAL_LIFELINES];

public:
    GameLogic();

    vector<int> apply50_50Lifeline(const Question& question);
    int applyAskFriendLifeline(const Question& question);
    bool applySkipLifeline();
    string applyHintLifeline(const Question& question); // New hint method
    bool isLifelineAvailable(int lifelineType) const;
    void displayLifelineStatus() const;
    void resetLifelines();
    int calculatePoints(int level, int difficulty);
    int getStreakBonus(int correctAnswers);
    int getNextDifficulty(int currentLevel);
    int getTimeLimit(int difficulty);
    bool validateAnswer(const Question& question, int selectedIndex) const;
    string getHint(const Question& question) const;
    void reset();
    int getTotalLifelinesUsed() const;
};

#endif
