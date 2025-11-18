#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>
#include <iostream>
using namespace std;
// Question Structure
struct Question {
    int id;
    int category;
     string text;
     vector< string> options;
    int correctAnswerIndex;
     string hint;
};

// Prize Level Structure (for Linked List)
struct PrizeNode {
    int level;
    long long prizeAmount;
    bool isSafetyLevel;
    PrizeNode* next;
    PrizeNode* prev;
    
    PrizeNode(int lvl, long long prize, bool safety = false)
        : level(lvl), prizeAmount(prize), isSafetyLevel(safety), next(nullptr), prev(nullptr) {}
};

// Doubly Linked List for Prize Ladder
class PrizeLadder {
private:
    PrizeNode* head;
    PrizeNode* current;
    int totalLevels;

public:
    PrizeLadder() : head(nullptr), current(nullptr), totalLevels(0) {
        initializeLadder();
    }

    ~PrizeLadder() {
        PrizeNode* temp = head;
        while (temp) {
            PrizeNode* next = temp->next;
            delete temp;
            temp = next;
        }
    }

    void initializeLadder() {
        long long prizes[] = {0, 1000, 2000, 5000, 10000, 20000, 40000, 80000, 160000, 320000, 640000, 1250000, 2500000, 5000000, 10000000, 50000000};
        bool safetyLevels[] = {false, false, false, true, false, false, false, true, false, false, false, true, false, false, false, true};
        
        for (int i = 0; i < 16; i++) {
            PrizeNode* newNode = new PrizeNode(i, prizes[i], safetyLevels[i]);
            if (!head) {
                head = newNode;
                current = head;
            } else {
                PrizeNode* temp = head;
                while (temp->next) temp = temp->next;
                temp->next = newNode;
                newNode->prev = temp;
            }
            totalLevels++;
        }
    }

    void moveForward() {
        if (current && current->next) {
            current = current->next;
        }
    }

    void moveToSafetyLevel() {
        if (current && current->prev) {
            PrizeNode* temp = current->prev;
            while (temp && !temp->isSafetyLevel) {
                temp = temp->prev;
            }
            if (temp) current = temp;
        }
    }

    long long getCurrentPrize() const {
        return current ? current->prizeAmount : 0;
    }

    int getCurrentLevel() const {
        return current ? current->level : 0;
    }

    bool isSafetyLevel() const {
        return current ? current->isSafetyLevel : false;
    }

    PrizeNode* getHead() const { return head; }
    PrizeNode* getCurrent() const { return current; }
};

// Player Structure
struct Player {
     string name;
     string gender;
    long long totalWinnings;
    int questionsAnswered;
    int currentLevel;
     unordered_map<int, bool> questionsAsked;
    int lifelinesUsed[3]; // 0: 50-50, 1: Ask Friend, 2: Skip

    Player() : totalWinnings(0), questionsAnswered(0), currentLevel(0) {
        lifelinesUsed[0] = 0;
        lifelinesUsed[1] = 0;
        lifelinesUsed[2] = 0;
    }

    void recordQuestion(int qID) {
        questionsAsked[qID] = true;
    }

    bool hasAskedQuestion(int qID) const {
        return questionsAsked.find(qID) != questionsAsked.end();
    }
};

// Stack for Lifelines Management
class LifelineStack {
private:
     stack<int> lifelineHistory; // 0: 50-50, 1: Ask Friend, 2: Skip

public:
    void usedLifeline(int type) {
        lifelineHistory.push(type);
    }

    bool canUseLifeline(int type, const Player& player) const {
        return player.lifelinesUsed[type] == 0;
    }

    int getLifelineCount() const {
        return lifelineHistory.size();
    }
};

// Max-Heap for Difficulty-based Question Selection
class QuestionPriorityQueue {
private:
     vector<Question> heap;

    void bubbleUp(int index) {
        while (index > 0 && heap[(index - 1) / 2].category < heap[index].category) {
             swap(heap[index], heap[(index - 1) / 2]);
            index = (index - 1) / 2;
        }
    }

    void bubbleDown(int index) {
        int size = heap.size();
        while (2 * index + 1 < size) {
            int larger = 2 * index + 1;
            if (2 * index + 2 < size && heap[2 * index + 2].category > heap[larger].category) {
                larger = 2 * index + 2;
            }
            if (heap[index].category < heap[larger].category) {
                 swap(heap[index], heap[larger]);
                index = larger;
            } else {
                break;
            }
        }
    }

public:
    void insert(const Question& q) {
        heap.push_back(q);
        bubbleUp(heap.size() - 1);
    }

    Question extractMax() {
        Question max = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) bubbleDown(0);
        return max;
    }

    bool isEmpty() const { return heap.empty(); }
};

#endif
