#pragma once
#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <random>
#include "Question.h"

// Global RNG
extern std::mt19937 GLOBAL_RNG;

// Split by '|'
std::vector<std::string> splitPipe(const std::string& line);

// Pop next *unused* question from heap
struct PQItem {
    int difficulty;
    int seq;
    Question q;
    bool operator<(PQItem const& other) const {
        if (difficulty != other.difficulty)
            return difficulty < other.difficulty;
        return seq > other.seq;
    }
};

bool getNextQuestionFromHeap(
    std::priority_queue<PQItem>& heap,
    std::unordered_set<std::string>& asked,
    PQItem& out
);