// src/PrizeLadder.cpp

#include "PrizeLadder.h"
#include <iomanip> // Used for output formatting

// --- Private Methods ---

void PrizeLadder::initializeLadder() {
    // DSA Usage:
    // Algorithm: Direct insertion (A) into the map, taking O(log N) time per insertion.
    ladder[1]  = "1,000";
    ladder[2]  = "2,000";
    ladder[3]  = "3,000";
    ladder[4]  = "5,000";
    ladder[5]  = "10,000";     // SAFE POINT 1
    ladder[6]  = "20,000";
    ladder[7]  = "40,000";
    ladder[8]  = "80,000";
    ladder[9]  = "1,60,000";
    ladder[10] = "3,20,000";   // SAFE POINT 2
    ladder[11] = "6,40,000";
    ladder[12] = "12,50,000";
    ladder[13] = "25,00,000";
    ladder[14] = "50,00,000";
    ladder[15] = "7,00,00,000"; // JACKPOT PRIZE
}

// --- Public Methods ---

PrizeLadder::PrizeLadder() {
    initializeLadder();
}

std::string PrizeLadder::getPrize(int level) const {
    // DSA Usage:
    // Algorithm: Map Lookup (A) using .count() and .at() for O(log N) retrieval time.
    if (ladder.count(level)) {
        return ladder.at(level);
    }
    return "0";
}

bool PrizeLadder::isSafePoint(int level) const {
    // DSA Usage:
    // Algorithm: Constant Time Check (A) - O(1) time complexity.
    return (level == 5 || level == 10);
}

std::string PrizeLadder::getSafePrize(int lastCompletedLevel) const {
    // DSA Usage:
    // Algorithm: Conditional Logic (A) determines the safe prize based on the level. O(1) time.
    if (lastCompletedLevel >= 10) {
        return getPrize(10); 
    } else if (lastCompletedLevel >= 5) {
        return getPrize(5);
    } else {
        return "0";
    }
}

void PrizeLadder::displayLadder() const {
    std::cout << "\n--- KBC Prize Ladder ---\n";
    for (int level = 15; level >= 1; --level) { 
        std::string prize = getPrize(level);
        // Using iomanip::setw for presentation style
        std::cout << "Level " << std::setw(2) << level << ": " << std::setw(12) << std::right << prize;
        if (isSafePoint(level)) {
            std::cout << "  (SAFE POINT)";
        } else if (level == 15) {
            std::cout << "  (JACKPOT)";
        }
        std::cout << "\n";
    }
    std::cout << "--------------------------\n";
}