#include "game_controller.hpp"
#include "game_engine.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int main() {
    GameEngine engine;

    if (!engine.initialize("questions.txt")) {
        cerr << "Failed to load questions. Make sure 'questions.txt' exists in the same directory.\n";
        return 1;
    }

    GameController controller(engine);

    while (!controller.isExiting()) {
        if (controller.getState() == GameState::MENU) {
            clearScreen();
        }
        controller.update();
    }

    cout << "\n╔════════════════════════════════════════════════════╗\n";
    cout << "║              THANKS FOR PLAYING!                   ║\n";
    cout << "║         Who Wants to Be a Millionaire - DSA        ║\n";
    cout << "╠════════════════════════════════════════════════════╣\n";
    engine.getLeaderboard().display();
    cout << "Goodbye!\n\n";

    return 0;
}
