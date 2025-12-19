#include "game_logic.hpp"

using namespace std;

GameLogic::LifelineState::LifelineState() : used(false), usageCount(0) {}

GameLogic::GameLogic() {
    srand(static_cast<unsigned>(time(0)));
}

vector<int> GameLogic::apply50_50Lifeline(const Question& question) {
    if (lifelineStates[FIFTY_FIFTY].used) {
        return {};
    }

    vector<int> wrongIndices;
    for (int i = 0; i < 4; i++) {
        if (i != question.correctAnswerIndex) {
            wrongIndices.push_back(i);
        }
    }

    for (int i = wrongIndices.size() - 1; i > 0; --i) {
        int j = rand() % (i + 1);
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

int GameLogic::applyAskFriendLifeline(const Question& question) {
    if (lifelineStates[ASK_FRIEND].used) {
        return -1;
    }

    int accuracy = rand() % 100;
    int suggestion;

    if (accuracy < 85) {
        suggestion = question.correctAnswerIndex;
    } else {
        do {
            suggestion = rand() % 4;
        } while (suggestion == question.correctAnswerIndex);
    }

    lifelineStates[ASK_FRIEND].used = true;
    lifelineStates[ASK_FRIEND].usageCount++;

    return suggestion;
}

bool GameLogic::applySkipLifeline() {
    if (lifelineStates[SKIP].used) {
        return false;
    }

    lifelineStates[SKIP].used = true;
    lifelineStates[SKIP].usageCount++;
    return true;
}

string GameLogic::applyHintLifeline(const Question& question) {
    if (lifelineStates[HINT].used) {
        return "";
    }

    lifelineStates[HINT].used = true;
    lifelineStates[HINT].usageCount++;

    return question.hint;
}

bool GameLogic::isLifelineAvailable(int lifelineType) const {
    if (lifelineType < 0 || lifelineType >= TOTAL_LIFELINES) {
        return false;
    }
    return !lifelineStates[lifelineType].used;
}

void GameLogic::displayLifelineStatus() const {
    cout << "Available Lifelines:\n";
    cout << "  1. 50-50: " << (isLifelineAvailable(FIFTY_FIFTY) ? "Available" : "Used") << "\n";
    cout << "  2. Ask Friend: " << (isLifelineAvailable(ASK_FRIEND) ? "Available" : "Used") << "\n";
    cout << "  3. Skip: " << (isLifelineAvailable(SKIP) ? "Available" : "Used") << "\n";
    cout << "  4. Hint: " << (isLifelineAvailable(HINT) ? "Available" : "Used") << "\n"; // 
}

void GameLogic::resetLifelines() {
    for (int i = 0; i < TOTAL_LIFELINES; i++) {
        lifelineStates[i].used = false;
        lifelineStates[i].usageCount = 0;
    }
}

int GameLogic::calculatePoints(int level, int difficulty) {
    int basePoints = 100;
    return basePoints * (level + 1) * (difficulty / 2 + 1);
}

int GameLogic::getStreakBonus(int correctAnswers) {
    if (correctAnswers < 3) return 0;
    return (correctAnswers - 2) * 50;
}

int GameLogic::getNextDifficulty(int currentLevel) {
    if (currentLevel < 5) return 1;
    if (currentLevel < 10) return 2;
    return 3;
}

int GameLogic::getTimeLimit(int difficulty) {
    switch (difficulty) {
        case 1: return 45;
        case 2: return 35;
        case 3: return 25;
        default: return 30;
    }
}

bool GameLogic::validateAnswer(const Question& question, int selectedIndex) const {
    return selectedIndex == question.correctAnswerIndex;
}

string GameLogic::getHint(const Question& question) const {
    return question.hint;
}

void GameLogic::reset() {
    resetLifelines();
}

int GameLogic::getTotalLifelinesUsed() const {
    int total = 0;
    for (int i = 0; i < TOTAL_LIFELINES; i++) {
        if (lifelineStates[i].used) total++;
    }
    return total;
}
