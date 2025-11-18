#ifdef RAYLIB_ENABLED

#include "gui_renderer.h"
#include "game_engine.h"
#include <iostream>
using namespace std;
int main() {
    GUIRenderer renderer;
    GameEngine engine;

    if (!engine.initialize("questions.txt")) {
         cerr << "Failed to load questions\n";
        return 1;
    }

    bool inGame = false;
    bool inLeaderboard = false;
     string playerName;
     string playerGender;

    while (!renderer.windowShouldClose()) {
        renderer.beginDrawing();

        if (!inGame && !inLeaderboard) {
            // Main Menu
            renderer.drawMainMenu();

            // Check for button clicks (simplified for raylib)
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                // Click detection would go here
            }
        } else if (inGame) {
            // Game Screen
            renderer.drawGameScreen(engine);
        } else if (inLeaderboard) {
            // Leaderboard Screen
            renderer.drawLeaderboardScreen(engine.getLeaderboard());
        }

        renderer.endDrawing();
    }

    return 0;
}

#endif
