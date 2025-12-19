#ifndef RAYLIB_RENDERER_HPP
#define RAYLIB_RENDERER_HPP

#include "raylib.h"
#include "game_controller.hpp"
#include "game_engine.hpp"
#include "game_state.hpp"
#include "buttons.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

// --- GLOBAL UI ASSET DEFINITIONS ---
struct RendererAssets
{
    // Images
    Texture2D splashBg;
    Texture2D setupBg;

    Texture2D menuBg; // Added based on context
    
    // Intro Video Assets REMOVED

    Texture2D bgMaleZoomOut;
    Texture2D bgMaleZoomIn;
    Texture2D bgFemaleZoomOut;
    Texture2D bgFemaleZoomIn;

    Texture2D bgGameOver;
    Texture2D bgWin; // NEW: Win Background
    Texture2D bgFinalScore;

    Texture2D icon5050;
    Texture2D iconPhone;
    Texture2D iconSkip;
    Texture2D iconHint;

    // Fonts
    Font gameFont;

    // Audio - Music (Loops)
    Music menuSound;
    Music musicTimer;       // Sound 1: 10s Countdown Loop
    Music musicLeaderboard; // Sound 5: Leaderboard Loop

    // Audio - SFX (One Shot)
    Sound sfxCorrect;       // Sound 2
    Sound sfxWrong;         // Sound 3
    Sound sfxGameOver;      // Sound 4
    Sound sfxWin;           // Sound 6
    Sound sfxLifeline;      // Sound 7 (Phone)

    // Buttons
    Button *playButton;
    Button *leaderButton;
    Button *exitButton;

    // Option buttons
    Button *optionButtons[4];
    Button *lifelineButtons[4]; 
};

/**
 * @brief Handles all Raylib drawing and input for the GUI layer.
 */
class RaylibRenderer
{
private:
    GameController &controller;
    GameEngine &engine;
    RendererAssets assets;

    RenderTexture2D target;
    const int virtualWidth = 1920;
    const int virtualHeight = 1080;
    float scale;
    Vector2 offset;

    // Startup Fade Variable
    float startupAlpha = 1.0f;
    float buttonAlpha = 0.0f;       // For Menu Fade-in

    const int MAX_NAME_LENGTH = 16;
    char playerNameBuffer[17] = "\0";
    char playerGenderBuffer[3] = "\0";
    int letterCountName = 0;
    int letterCountGender = 0;
    int activeTextBox = 0;
    Rectangle nameBox;
    Rectangle genderBox;

    Rectangle lifelineRects[4];

    std::string cachedQuote;
    std::string cachedResultText;

    // State Tracking for Sound Triggers
    GameState previousState;

    // --- Private Drawing Methods ---
    void drawAndHandleSplash();
    void drawAndHandleMenu(Vector2 mousePos, bool mousePressed);
    void drawAndHandlePlayerSetup(Vector2 mousePos, bool mousePressed, int key);
    void drawAndHandleGameIntro();
    void drawAndHandleGameplay(Vector2 mousePos, bool mousePressed);

    void drawLeaderboard();
    void drawResultDisplay();
    void drawGameOverScreen();
    void drawAndHandleFinalScore();

    void drawPrizeLadder();

    void drawTimer();
    void drawLifelinePopup();

    // Helper to manage sound transitions
    void handleStateAudio(GameState currentState);

    void drawCenteredText(const char *text, int y, int fontSize, Color color) const;
    void drawTextEx(const char *text, float x, float y, float fontSize, Color color) const;

    std::string formatMoney(long long amount) const;

    void handleTextInput(char *buffer, int &count, int max_length, int key, bool active);
    Vector2 getVirtualMousePosition();

    string getRandomResultPhrase(bool isCorrect);

public:
    RaylibRenderer(GameController &ctrl, GameEngine &eng);
    ~RaylibRenderer();

    void updateAndDraw();
};

#endif