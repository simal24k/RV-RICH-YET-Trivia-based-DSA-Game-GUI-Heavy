#include "BaseFunctions.h"
#include <ctime>

std::mt19937 GLOBAL_RNG((unsigned)std::time(nullptr));

std::vector<std::string> splitPipe(const std::string &line) {
    std::vector<std::string> parts;
    std::string cur;
    for (char c : line) {
        if (c == '|') {
            parts.push_back(cur);
            cur.clear();
        } else cur.push_back(c);
    }
    parts.push_back(cur);
    return parts;
}

bool getNextQuestionFromHeap(
    std::priority_queue<PQItem>& heap,
    std::unordered_set<std::string>& asked,
    PQItem& out
) {
    while (!heap.empty()) {
        PQItem top = heap.top(); heap.pop();
        if (asked.find(top.q.id) == asked.end()) {
            out = top;
            return true;
        }
    }
    return false;
}
