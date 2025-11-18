#include "game_controller.h"
#include "game_engine.h"
#include <iostream>
#include <iomanip>
#include <raylib.h>


using namespace std;


void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void displayMainMenu() {
    clearScreen();
     cout << "\n";
     cout << "╔════════════════════════════════════════════════════╗\n";
     cout << "║   WHO WANTS TO BE A MILLIONAIRE - DSA Edition     ║\n";
     cout << "║                                                    ║\n";
     cout << "║              1. PLAY GAME                          ║\n";
     cout << "║              2. VIEW LEADERBOARD                   ║\n";
     cout << "║              3. EXIT                               ║\n";
     cout << "║                                                    ║\n";
     cout << "╚════════════════════════════════════════════════════╝\n";
}

void displayStatsMenu() {
    clearScreen();
     cout << "\n";
     cout << "╔════════════════════════════════════════════════════╗\n";
     cout << "║            ADDITIONAL OPTIONS                      ║\n";
     cout << "║                                                    ║\n";
     cout << "║              1. VIEW PLAYER PROFILE                ║\n";
     cout << "║              2. VIEW ALL PLAYERS                   ║\n";
     cout << "║              3. VIEW LEADERBOARD STATS             ║\n";
     cout << "║              4. RETURN TO MAIN MENU                ║\n";
     cout << "║                                                    ║\n";
     cout << "╚════════════════════════════════════════════════════╝\n";
}

int main() {
    GameEngine engine;

    if (!engine.initialize("questions.txt")) {
         cerr << "Failed to load questions. Make sure 'questions.txt' exists.\n";
        return 1;
    }

    GameController controller(engine);

    while (!controller.isExiting()) {
        if (controller.getState() == GameState::MENU) {
            displayMainMenu();
        } else if (controller.getState() == GameState::MENU) {
            displayStatsMenu();
        }
        controller.update();
    }

     cout << "\n                                                    \n";
     cout << "              THANKS FOR PLAYING!                    \n";
     cout << "                                                      \n";
    engine.getLeaderboard().display();
     cout << "\nGoodbye!\n\n";

    return 0;
}
