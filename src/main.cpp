#include <iostream>
#include "game_controller.hpp"
#include "game_engine.hpp"
#include "game_state.hpp"
#include "raylib_renderer.hpp"

using namespace std;

int main(int, char **)
{
   
    GameEngine engine;

    if (!engine.initialize("docs/questions.txt"))
    {
        cerr << "Failed to load questions. Make sure 'docs/questions.txt' exists.\n";
        return 1;
    }

    GameController controller(engine);

   
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "RV Rich Yet?");
    MaximizeWindow();

    InitAudioDevice();
    SetTargetFPS(60);

    
    RaylibRenderer renderer(controller, engine);

    controller.setState(GameState::SPLASH);

    while (!WindowShouldClose() && controller.getState() != GameState::EXIT)
    {
        renderer.updateAndDraw();
    }

   
    CloseAudioDevice();
    CloseWindow();

    cout << "Goodbye!\n\n";

    return 0;
}