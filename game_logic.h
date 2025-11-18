#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "data_structures.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;

class GameLogic {
private:
    static constexpr int TOTAL_LIFELINES = 3;
    
    enum LifelineType {
        FIFTY_FIFTY = 0,
        ASK_FRIEND = 1,
        SKIP = 2
    };

    struct LifelineState {
        bool used;
        int usageCount;
        
        LifelineState() : used(false), usageCount(0) {}
    };

    LifelineState lifelineStates[TOTAL_LIFELINES];

public:
    GameLogic() {
         srand(static_cast<unsigned>( time(0)));
    }

    // Apply 50-50 lifeline - removes 2 wrong answers
     vector<int> apply50_50Lifeline(const Question& question) {
        if (lifelineStates[FIFTY_FIFTY].used) {
            return {};
        }

         vector<int> wrongIndices;
        for (int i = 0; i < 4; i++) {
            if (i != question.correctAnswerIndex) {
                wrongIndices.push_back(i);
            }
        }

        // Shuffle wrong answers and remove 2
        for (int i = wrongIndices.size() - 1; i > 0; --i) {
            int j =  rand() % (i + 1);
             swap(wrongIndices[i], wrongIndices[j]);
        }

         vector<int> removedAnswers;
        if (wrongIndices.size() >= 2) {
            removedAnswers.push_back(wrongIndices[0]);
            removedAnswers.push_back(wrongIndices[1]);
        }

        lifelineStates[FIFTY_FIFTY].used = true;
        lifelineStates[FIFTY_FIFTY].usageCount++;

        return removedAnswers;
    }

    // Apply Ask Friend lifeline - friend suggests correct answer
    int applyAskFriendLifeline(const Question& question) {
        if (lifelineStates[ASK_FRIEND].used) {
            return -1;
        }

        // Friend has 85% accuracy
        int accuracy =  rand() % 100;
        int suggestion;

        if (accuracy < 85) {
            suggestion = question.correctAnswerIndex;
        } else {
            // Wrong suggestion
            do {
                suggestion =  rand() % 4;
            } while (suggestion == question.correctAnswerIndex);
        }

        lifelineStates[ASK_FRIEND].used = true;
        lifelineStates[ASK_FRIEND].usageCount++;

        return suggestion;
    }

    // Apply Skip lifeline - skip current question without penalty
    bool applySkipLifeline() {
        if (lifelineStates[SKIP].used) {
            return false;
        }

        lifelineStates[SKIP].used = true;
        lifelineStates[SKIP].usageCount++;
        return true;
    }

    // Check if a lifeline is available
    bool isLifelineAvailable(int lifelineType) const {
        if (lifelineType < 0 || lifelineType >= TOTAL_LIFELINES) {
            return false;
        }
        return !lifelineStates[lifelineType].used;
    }

    // Get lifeline status
    void displayLifelineStatus() const {
         cout << "Available Lifelines:\n";
         cout << "  1. 50-50: " << (isLifelineAvailable(FIFTY_FIFTY) ? "Available" : "Used") << "\n";
         cout << "  2. Ask Friend: " << (isLifelineAvailable(ASK_FRIEND) ? "Available" : "Used") << "\n";
         cout << "  3. Skip: " << (isLifelineAvailable(SKIP) ? "Available" : "Used") << "\n";
    }

    // Reset lifelines for new game
    void resetLifelines() {
        for (int i = 0; i < TOTAL_LIFELINES; i++) {
            lifelineStates[i].used = false;
            lifelineStates[i].usageCount = 0;
        }
    }

    // Calculate points based on difficulty
    int calculatePoints(int level, int difficulty) {
        int basePoints = 100;
        return basePoints * (level + 1) * (difficulty / 2 + 1);
    }

    // Calculate streak bonus
    int getStreakBonus(int correctAnswers) {
        if (correctAnswers < 3) return 0;
        return (correctAnswers - 2) * 50;
    }

    // Difficulty escalation based on level
    int getNextDifficulty(int currentLevel) {
        if (currentLevel < 5) return 1;      // Easy
        if (currentLevel < 10) return 2;     // Medium
        return 3;                            // Hard
    }

    // Time allocation based on difficulty
    int getTimeLimit(int difficulty) {
        switch (difficulty) {
            case 1: return 45;  // Easy: 45 seconds
            case 2: return 35;  // Medium: 35 seconds
            case 3: return 25;  // Hard: 25 seconds
            default: return 30;
        }
    }

    // Validate answer against correct answer
    bool validateAnswer(const Question& question, int selectedIndex) const {
        return selectedIndex == question.correctAnswerIndex;
    }

    // Get hint for current question
     string getHint(const Question& question) const {
        return question.hint;
    }

    // Reset all state for debugging/testing
    void reset() {
        resetLifelines();
    }

    // Get total lifelines used
    int getTotalLifelinesUsed() const {
        int total = 0;
        for (int i = 0; i < TOTAL_LIFELINES; i++) {
            if (lifelineStates[i].used) total++;
        }
        return total;
    }
};

#endif
