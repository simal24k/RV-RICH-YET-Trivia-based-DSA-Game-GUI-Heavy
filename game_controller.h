#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "game_engine.h"
#include "game_state.h"
#include "game_logic.h"
#include "timer.h"
#include <iostream>
#include <string>
#include <limits>
using namespace std;

class GameController {
private:
    GameEngine& engine;
    GameStateManager stateManager;
    GameLogic gameLogic;
    GameTimer currentTimer;
     string inputBuffer;
    bool waitingForInput;

public:
    GameController(GameEngine& gameEngine) 
        : engine(gameEngine), waitingForInput(true), currentTimer(30) {}

    void handleMenuInput() {
         cout << "\nSelect Option (1-3): ";
        int choice;
        while (!( cin >> choice) || choice < 1 || choice > 3) {
             cin.clear();
             cin.ignore( numeric_limits< streamsize>::max(), '\n');
             cout << "Invalid choice. Please enter 1-3: ";
        }

        switch (choice) {
            case 1:
                stateManager.setState(GameState::PLAYER_SETUP);
                break;
            case 2:
                stateManager.setState(GameState::LEADERBOARD);
                break;
            case 3:
                stateManager.setState(GameState::EXIT);
                break;
        }
    }

    void handlePlayerSetup() {
         string name, gender;
        
         cout << "Enter your name: ";
         cin.ignore();
         getline( cin, name);

         cout << "Enter your gender (M/F): ";
         getline( cin, gender);

        engine.setupPlayer(name, gender);
        stateManager.setState(GameState::QUESTION_DISPLAY);
    }

    void handleQuestionDisplay() {
        if (!engine.getNextQuestion()) {
             cout << "No more questions available!\n";
            stateManager.setState(GameState::GAME_OVER);
            return;
        }

        engine.displayGameStatus();
        engine.displayQuestion();
        
        currentTimer.setDuration(gameLogic.getTimeLimit(
            gameLogic.getNextDifficulty(engine.getPlayer().currentLevel)
        ));
        currentTimer.start();

        stateManager.setState(GameState::ANSWER_PROCESSING);
    }

    void handleAnswerProcessing() {
         cout << "\nEnter option (A-D) or use lifeline (5:50-50, 6:AskFriend, 7:Skip, 0:Quit): ";
        
        char input;
         cin >> input;
        input =  toupper(input);

        if (input == '0') {
            stateManager.setResult(GameResult::QUIT);
            stateManager.setState(GameState::GAME_OVER);
            return;
        }

        // Check for lifeline usage
        if (input == '5') {
            handleLifelineUsage(0);
            stateManager.setState(GameState::ANSWER_PROCESSING);
            return;
        }

        if (input == '6') {
            handleLifelineUsage(1);
            stateManager.setState(GameState::ANSWER_PROCESSING);
            return;
        }

        if (input == '7') {
            handleLifelineUsage(2);
            stateManager.setState(GameState::QUESTION_DISPLAY);
            return;
        }

        // Process answer
        if (input >= 'A' && input <= 'D') {
            int optionIndex = input - 'A';
            bool isCorrect = engine.processAnswer(optionIndex);

            if (isCorrect) {
                stateManager.setResult(GameResult::CORRECT);
            } else {
                stateManager.setResult(GameResult::WRONG);
            }

            stateManager.setState(GameState::RESULT_DISPLAY);
        } else {
             cout << "Invalid input. Please try again.\n";
        }
    }

    void handleResultDisplay() {
        const auto& player = engine.getPlayer();
        
         cout << "\n╔════════════════════════════════════════════════════╗\n";
        
        if (stateManager.getLastResult() == GameResult::CORRECT) {
             cout << "║              ✓ CORRECT ANSWER!                     ║\n";
             cout << "║  You just won: Rs. " <<  setw(28) << player.totalWinnings << " ║\n";
        } else if (stateManager.getLastResult() == GameResult::WRONG) {
             cout << "║              ✗ WRONG ANSWER!                      ║\n";
             cout << "║  Correct answer: " << (char)('A' + engine.getCurrentQuestion().correctAnswerIndex) 
                      <<  string(41 - 22, ' ') << " ║\n";
             cout << "║  You take home: Rs. " <<  setw(25) << player.totalWinnings << " ║\n";
            stateManager.setState(GameState::GAME_OVER);
            return;
        }

         cout << "╚════════════════════════════════════════════════════╝\n";
         cout << "Press Enter for next question...";
         cin.ignore();
         cin.ignore();

        if (stateManager.getLastResult() == GameResult::CORRECT) {
            stateManager.setState(GameState::QUESTION_DISPLAY);
        }
    }

    void handleGameOver() {
        engine.endGame();
        
        const auto& player = engine.getPlayer();
        
         cout << "\n╔════════════════════════════════════════════════════╗\n";
         cout << "║                 GAME SUMMARY                       ║\n";
         cout << "║                                                    ║\n";
         cout << "║  Final Winnings: Rs. " <<  setw(25) << player.totalWinnings << " ║\n";
         cout << "║  Level Reached: " <<  setw(30) << player.currentLevel << " ║\n";
         cout << "║  Questions Answered: " <<  setw(25) << player.questionsAnswered << " ║\n";
         cout << "║                                                    ║\n";
         cout << "╚════════════════════════════════════════════════════╝\n";

         cout << "Press Enter to return to main menu...";
         cin.ignore();
         cin.ignore();

        stateManager.setState(GameState::MENU);
    }

    void handleLeaderboard() {
        engine.getLeaderboard().display();
        
         cout << "Press Enter to return to main menu...";
         cin.ignore();
         cin.ignore();

        stateManager.setState(GameState::MENU);
    }

private:
    void handleLifelineUsage(int lifelineType) {
        if (lifelineType == 0) {
            auto removedAnswers = engine.use50_50Lifeline();
            if (!removedAnswers.empty()) {
                 cout << "\nLifeline: 50-50\n";
                 cout << "Two wrong answers eliminated:\n";
                for (int idx : removedAnswers) {
                     cout << "  - " << (char)('A' + idx) << ": " 
                              << engine.getCurrentQuestion().options[idx] << "\n";
                }
            } else {
                 cout << "\n50-50 lifeline already used!\n";
            }
        } else if (lifelineType == 1) {
            int suggestion = engine.useAskFriendLifeline();
            if (suggestion != -1) {
                 cout << "\nLifeline: Ask a Friend\n";
                 cout << "Your friend suggests: " << (char)('A' + suggestion) << "\n";
            } else {
                 cout << "\nAsk Friend lifeline already used!\n";
            }
        } else if (lifelineType == 2) {
            if (engine.useSkipLifeline()) {
                 cout << "\nQuestion skipped!\n";
                 this_thread::sleep_for( chrono::seconds(2));
            } else {
                 cout << "\nSkip lifeline already used!\n";
            }
        }
    }

public:
    void update() {
        switch (stateManager.getState()) {
            case GameState::MENU:
                handleMenuInput();
                break;
            case GameState::PLAYER_SETUP:
                handlePlayerSetup();
                break;
            case GameState::QUESTION_DISPLAY:
                handleQuestionDisplay();
                break;
            case GameState::ANSWER_PROCESSING:
                handleAnswerProcessing();
                break;
            case GameState::RESULT_DISPLAY:
                handleResultDisplay();
                break;
            case GameState::GAME_OVER:
                handleGameOver();
                break;
            case GameState::LEADERBOARD:
                handleLeaderboard();
                break;
            case GameState::EXIT:
                break;
        }
    }

    GameState getState() const { return stateManager.getState(); }
    bool isExiting() const { return stateManager.getState() == GameState::EXIT; }
};

#endif
