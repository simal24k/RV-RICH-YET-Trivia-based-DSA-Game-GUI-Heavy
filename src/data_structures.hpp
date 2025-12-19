#ifndef DATA_STRUCTURES_HPP
#define DATA_STRUCTURES_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>
#include <iostream>

using namespace std;

struct Question {
    int id;
    int category;
    string text;
    vector<string> options;
    int correctAnswerIndex;
    string hint;
};


struct PrizeNode {
    int level;
    long long prizeAmount;
    bool isSafetyLevel;
    PrizeNode* next;
    PrizeNode* prev;
    
    PrizeNode(int lvl, long long prize, bool safety = false)
        : level(lvl), prizeAmount(prize), isSafetyLevel(safety), next(nullptr), prev(nullptr) {}
};


class PrizeLadder {
private:
    PrizeNode* head;
    PrizeNode* current;
    int totalLevels;

public:
    PrizeLadder();
    ~PrizeLadder();
    
    void initializeLadder();
    void moveForward();
    void moveToSafetyLevel();
    void resetLadder(); 
    long long getCurrentPrize() const;
    int getCurrentLevel() const;
    bool isSafetyLevel() const;
    PrizeNode* getHead() const;
    PrizeNode* getCurrent() const;
};

// Player Structure
struct Player {
    string name;
    string gender;
    long long totalWinnings;
    int questionsAnswered;
    int currentLevel;
    unordered_map<int, bool> questionsAsked;
    int lifelinesUsed[4]; // 0: 50-50, 1: Ask Friend, 2: Skip, 3: Hint

    Player();
    void recordQuestion(int qID);
    bool hasAskedQuestion(int qID) const;
};


class LifelineStack {
private:
    stack<int> lifelineHistory; 
public:
    void usedLifeline(int type);
    bool canUseLifeline(int type, const Player& player) const;
    int getLifelineCount() const;
};

class QuestionPriorityQueue {
private:
    vector<Question> heap;

    void bubbleUp(int index);
    void bubbleDown(int index);

public:
    void insert(const Question& q);
    Question extractMax();
    bool isEmpty() const;
};

#endif
