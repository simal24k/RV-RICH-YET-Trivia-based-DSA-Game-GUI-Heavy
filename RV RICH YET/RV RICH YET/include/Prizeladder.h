// include/PrizeLadder.h

#ifndef PRIZE_LADDER_H
#define PRIZE_LADDER_H

#include <string>
#include <map>
#include <iostream>

/**
 * @brief Manages the prize money structure, levels, and safe points for the game.
 * * DSA Usage:
 * Data Structure: std::map<int, std::string> (DS) is used to store the ladder.
 * The map uses a balanced tree (or similar structure, depending on the compiler) 
 * for its implementation, providing **O(log N)** time complexity for key lookup (level number).
 */
class PrizeLadder {
private:
    // Stores level (key) -> prize amount string (value)
    std::map<int, std::string> ladder; 

    /**
     * @brief Helper function to initialize the fixed prize ladder amounts.
     */
    void initializeLadder();

public:
    // --- Constructor ---
    PrizeLadder();

    // --- Core Methods ---
    
    /**
     * @brief Retrieves the prize amount for a given level.
     */
    std::string getPrize(int level) const;

    /**
     * @brief Checks if a given level is a safe milestone point (Level 5 or Level 10).
     */
    bool isSafePoint(int level) const;

    /**
     * @brief Retrieves the guaranteed amount if the player walks away or answers incorrectly.
     * @param lastCompletedLevel The level the player *completed*.
     * @return The guaranteed safe prize money as a string.
     */
    std::string getSafePrize(int lastCompletedLevel) const;

    /**
     * @brief Prints the entire prize ladder structure to the console.
     * * DSA Usage:
     * Algorithm: Iteration (A) is used to loop from level 15 down to 1 for display.
     */
    void displayLadder() const;
};

#endif // PRIZE_LADDER_H