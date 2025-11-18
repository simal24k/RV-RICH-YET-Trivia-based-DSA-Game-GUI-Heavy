#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "data_structures.h"
#include "question_bank.h"
#include "leaderboard.h"
#include "game_logic.h"
#include "timer.h"
#include "player_profile.h"
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

class GameEngine {
private:
    Player player;
    QuestionBank questionBank;
    PrizeLadder prizeLadder;
    LifelineStack lifelineStack;
    Leaderboard leaderboard;
    GameLogic gameLogic;
    GameTimer gameTimer;
    Question currentQuestion;
    int timeLimit;
    bool gameActive;
    int correctAnswerStreak;
    int totalPointsEarned;
    PlayerProfileManager playerProfileManager;

public:
    GameEngine() : timeLimit(30), gameActive(false), correctAnswerStreak(0), totalPointsEarned(0) {}

    bool initialize(const  string& questionsFile) {
        return questionBank.loadFromFile(questionsFile);
    }

    void setupPlayer(const  string& name, const  string& gender) {
        player.name = name;
        player.gender = gender;
        playerProfileManager.getOrCreateProfile(name, gender);
        gameLogic.resetLifelines();
        correctAnswerStreak = 0;
        totalPointsEarned = 0;
    }

    bool getNextQuestion() {
        currentQuestion = questionBank.getNextQuestion(player);
        if (currentQuestion.id == -1) return false;
        player.recordQuestion(currentQuestion.id);
        
        int difficulty = gameLogic.getNextDifficulty(player.currentLevel);
        timeLimit = gameLogic.getTimeLimit(difficulty);
        gameTimer.setDuration(timeLimit);
        gameTimer.start();
        
        return true;
    }

    bool processAnswer(int optionIndex) {
        bool isCorrect = questionBank.isCorrectAnswer(currentQuestion.id, optionIndex);
        
        if (isCorrect) {
            correctAnswerStreak++;
            int points = gameLogic.calculatePoints(player.currentLevel, 
                                                   gameLogic.getNextDifficulty(player.currentLevel));
            points += gameLogic.getStreakBonus(correctAnswerStreak);
            totalPointsEarned += points;
            
            prizeLadder.moveForward();
            player.currentLevel = prizeLadder.getCurrentLevel();
            player.totalWinnings = prizeLadder.getCurrentPrize();
            player.questionsAnswered++;
            return true;
        } else {
            correctAnswerStreak = 0;
            if (!prizeLadder.isSafetyLevel()) {
                prizeLadder.moveToSafetyLevel();
            }
            player.totalWinnings = prizeLadder.getCurrentPrize();
            return false;
        }
    }

     vector<int> use50_50Lifeline() {
        if (!gameLogic.isLifelineAvailable(0)) return {};
        
        player.lifelinesUsed[0] = 1;
        lifelineStack.usedLifeline(0);
        return gameLogic.apply50_50Lifeline(currentQuestion);
    }

    int useAskFriendLifeline() {
        if (!gameLogic.isLifelineAvailable(1)) return -1;
        
        player.lifelinesUsed[1] = 1;
        lifelineStack.usedLifeline(1);
        return gameLogic.applyAskFriendLifeline(currentQuestion);
    }

    bool useSkipLifeline() {
        if (!gameLogic.isLifelineAvailable(2)) return false;
        
        player.lifelinesUsed[2] = 1;
        lifelineStack.usedLifeline(2);
        return gameLogic.applySkipLifeline();
    }

    bool isLifelineAvailable(int lifelineType) const {
        return gameLogic.isLifelineAvailable(lifelineType);
    }

    void endGame() {
        gameActive = false;
        gameTimer.stop();
        
        playerProfileManager.updatePlayerStats(
            player.name,
            player.totalWinnings,
            player.currentLevel,
            player.questionsAnswered
        );
        
        auto now =  chrono::system_clock::now();
        auto time =  chrono::system_clock::to_time_t(now);
        leaderboard.addEntry(player,  ctime(&time));
    }

    void displayGameStatus() {
         cout << "\n========== GAME STATUS ==========\n";
         cout << "Player: " << player.name << " (" << player.gender << ")\n";
         cout << "Current Level: " << player.currentLevel << "\n";
         cout << "Current Winnings: Rs. " << player.totalWinnings << "\n";
         cout << "Questions Answered: " << player.questionsAnswered << "\n";
         cout << "Correct Streak: " << correctAnswerStreak << "\n";
         cout << "Total Points: " << totalPointsEarned << "\n";
         cout << "=================================\n";
    }

    void displayQuestion() {
         cout << "\n[Level " << player.currentLevel << "] Q" << currentQuestion.id << ": " 
                  << currentQuestion.text << "\n";
         cout << "Hint: " << currentQuestion.hint << "\n\n";
        for (int i = 0; i < 4; i++) {
             cout << (char)('A' + i) << ") " << currentQuestion.options[i] << "\n";
        }
         cout << "\nLifelines: ";
         cout << "50-50(" << (player.lifelinesUsed[0] ? "X" : "✓") << ") ";
         cout << "AskFriend(" << (player.lifelinesUsed[1] ? "X" : "✓") << ") ";
         cout << "Skip(" << (player.lifelinesUsed[2] ? "X" : "✓") << ")\n";
         cout << "Time: ";
        gameTimer.displayTimer();
         cout << "\n";
    }

    const Player& getPlayer() const { return player; }
    const PrizeLadder& getPrizeLadder() const { return prizeLadder; }
    const Question& getCurrentQuestion() const { return currentQuestion; }
    Leaderboard& getLeaderboard() { return leaderboard; }
    int getTotalPoints() const { return totalPointsEarned; }
    int getCorrectStreak() const { return correctAnswerStreak; }
    PlayerStats getPlayerProfile() const {
        if (playerProfileManager.playerExists(player.name)) {
            return playerProfileManager.getOrCreateProfile(player.name, player.gender);
        }
        return PlayerStats(player.name, player.gender);
    }
    PlayerProfileManager& getProfileManager() { return playerProfileManager; }
};

#endif
