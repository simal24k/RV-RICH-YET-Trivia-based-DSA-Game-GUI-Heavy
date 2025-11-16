// src/main.cpp - Minimal Data Infrastructure Verification

#include <iostream>
#include <vector>
#include <algorithm>
#include "Questionbank.h"
#include "PrizeLadder.h"
#include "ProfileManager.h" 
#include "Question.h"

int main() {
    std::cout << "--- Stage 2: Minimal Data Verification (QuestionBank & PrizeLadder) ---\n";
    
    // --- 1. QuestionBank Loading ---
    QuestionBank qb;
    if (!qb.loadFromFile("data/questions.txt")) {
        std::cerr << "FATAL: Failed to load questions. Check data/questions.txt format and path.\n";
        return 1;
    }
    
    // DSA Check: Builds a Hash Map for O(1) retrieval by difficulty
    qb.buildMaps(); 
    int total_loaded = qb.allQuestions().size();
    std::cout << "Successfully loaded and mapped " << total_loaded << " questions.\n";

    // --- 2. Test Question Retrieval and Display ---
    int difficulty_to_test = 5;
    const std::vector<Question>& level_qs = qb.getQuestionsByDifficulty(difficulty_to_test);
    
    std::cout << "\n--- Data Verification at Difficulty " << difficulty_to_test << " ---\n";
    std::cout << "Found " << level_qs.size() << " questions at this level.\n";
    
    if (!level_qs.empty()) {
        std::cout << "Displaying first question found:\n";
        // Calls the Question::display() method
        level_qs.front().display(); 
        std::cout << "\nQuestion ID: " << level_qs.front().getId() << "\n";
        std::cout << "Correct Answer Index: " << level_qs.front().getCorrectIndex() << "\n";
        std::cout << "Hint: " << level_qs.front().getHint() << "\n";
    } else {
        std::cout << "No questions found for Difficulty " << difficulty_to_test << ".\n";
    }

    // --- 3. PrizeLadder Check (Uses Linked List structure) ---
    PrizeLadder ladder;
    std::cout << "\nPrize for this level: " << ladder.getPrize(difficulty_to_test) << "\n";
    std::cout << "Prize for safety level (5): " << ladder.getSafePrize(5) << "\n";
    
    std::cout << "------------------------------------------\n";
    std::cout << "--- Stage 3: QuestionBank Fix & ProfileManager DSA Verification ---\n";

    // --- 1. QuestionBank Verification (Checking Fix) ---
    QuestionBank qb2;
    if (!qb2.loadFromFile("data/questions.txt")) {
        std::cerr << "FATAL: Failed to load questions. Check data/questions.txt format and path.\n";
        return 1;
    }
    qb2.buildMaps();
    int total_loaded = qb2.allQuestions().size();

    // Check for successful load (expecting around 150 questions)
    std::cout << "Successfully loaded and mapped " << total_loaded << " questions.\n";
    if (total_loaded < 10) {
        std::cout << "\nCRITICAL: The parser bug is NOT fixed. Check warnings above.\n";
        return 1;
    }

    // --- 2. ProfileManager Verification (DSA: Hash Table & Hash Set) ---
    ProfileManager pm;
    std::string testName = "DSA_Test_User";
    std::string qId1 = qb2.allQuestions().front().getId(); // Get the ID of the first loaded question

    pm.updateScore(testName, 1000);
    pm.markQuestionUsed(testName, qId1); // Mark Q1 as used (O(1) Hash Set insert)

    // Save to test persistence
    pm.save("data/profiles.txt");

    // Reload the ProfileManager to test persistence
    ProfileManager pm_reloaded;
    pm_reloaded.load("data/profiles.txt");

    std::cout << "\n--- Profile Manager Verification (DSA Check) ---\n";

    // Check profile data (O(1) Hash Table lookup)
    PlayerProfile& p = pm_reloaded.getOrCreate(testName);
    std::cout << "Profile: " << p.name
              << ", High Score: " << p.highScore
              << ", Games Played: " << p.gamesPlayed << "\n";

    // Check history (O(1) Hash Set lookup)
    bool isUsed1 = p.questionHistory.count(qId1);
    std::cout << "Question ID " << qId1 << " used? " << (isUsed1 ? "Yes (Correct)" : "No (Error)") << "\n";

    // Cleanup
    p.highScore = 0;
    p.gamesPlayed = 0;

    std::cout << "------------------------------------------\n";

    return 0;
}