#ifndef GUI_RENDERER_H
#define GUI_RENDERER_H

#ifdef RAYLIB_ENABLED
#include "raylib.h"
#include "data_structures.h"
#include "game_engine.h"
#include <string>
using namespace std;
class GUIRenderer {
private:
    static constexpr int WINDOW_WIDTH = 1200;
    static constexpr int WINDOW_HEIGHT = 800;
    static constexpr int FPS = 60;

    // Color scheme
    Color colorBackground;
    Color colorPrimary;
    Color colorAccent;
    Color colorCorrect;
    Color colorWrong;
    Color colorText;
    Color colorButtonHover;

    Font mainFont;
    Font titleFont;

    enum GameScreen {
        SCREEN_MENU,
        SCREEN_PLAYER_SETUP,
        SCREEN_GAME,
        SCREEN_LIFELINE_POPUP,
        SCREEN_GAME_OVER,
        SCREEN_LEADERBOARD
    };

    GameScreen currentScreen;

public:
    GUIRenderer() : currentScreen(SCREEN_MENU) {
        initializeColors();
        initializeWindow();
    }

    ~GUIRenderer() {
        if (mainFont.texture.id != 0) UnloadFont(mainFont);
        if (titleFont.texture.id != 0) UnloadFont(titleFont);
        CloseWindow();
    }

    void initializeColors() {
        colorBackground = {15, 15, 35, 255};      // Dark blue-black
        colorPrimary = {0, 150, 255, 255};        // Bright cyan
        colorAccent = {255, 200, 50, 255};        // Golden yellow
        colorCorrect = {50, 200, 100, 255};       // Green
        colorWrong = {255, 80, 80, 255};          // Red
        colorText = {240, 240, 240, 255};         // Light gray
        colorButtonHover = {0, 200, 255, 255};    // Lighter cyan
    }

    void initializeWindow() {
        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Who Wants to Be a Millionaire - DSA Edition");
        SetTargetFPS(FPS);
        
        // Load default fonts or create basic ones
        mainFont = GetFontDefault();
        titleFont = GetFontDefault();
    }

    bool windowShouldClose() const {
        return ::WindowShouldClose();
    }

    void beginDrawing() {
        BeginDrawing();
        ClearBackground(colorBackground);
    }

    void endDrawing() {
        EndDrawing();
    }

    void drawMainMenu() {
        drawCenteredText("WHO WANTS TO BE A MILLIONAIRE", WINDOW_WIDTH / 2, 100, 40, colorAccent);
        drawCenteredText("DSA Edition", WINDOW_WIDTH / 2, 160, 24, colorText);

        // Menu buttons
        Vector2 playBtn = drawButton("PLAY GAME", WINDOW_WIDTH / 2 - 150, 300, 300, 60, colorPrimary);
        Vector2 leaderBtn = drawButton("LEADERBOARD", WINDOW_WIDTH / 2 - 150, 400, 300, 60, colorPrimary);
        Vector2 exitBtn = drawButton("EXIT", WINDOW_WIDTH / 2 - 150, 500, 300, 60, colorWrong);

        // Draw instructions
        drawCenteredText("Test your knowledge and climb the prize ladder!", WINDOW_WIDTH / 2, 650, 16, colorText);
        drawCenteredText("Use strategic lifelines to reach Rs. 50,000,000", WINDOW_WIDTH / 2, 680, 16, colorText);
    }

    void drawGameScreen(const GameEngine& engine) {
        // Draw prize ladder on left
        drawPrizeLadder(engine.getPrizeLadder());

        // Draw current question in center
        drawQuestionPanel(engine.getCurrentQuestion(), engine.getPlayer());

        // Draw game status on right
        drawGameStatus(engine.getPlayer());

        // Draw lifelines
        drawLifelines(engine.getPlayer());

        // Draw timer
        drawTimer();
    }

    void drawPrizeLadder(const PrizeLadder& ladder) {
        int startX = 20;
        int startY = 100;
        int levelBoxWidth = 120;
        int levelBoxHeight = 35;
        int spacing = 5;

        DrawRectangle(startX - 5, startY - 5, levelBoxWidth + 10, 650, 
                      {40, 40, 60, 200});

        PrizeNode* node = ladder.getHead();
        int yPos = startY;

        while (node && yPos < startY + 650) {
            bool isCurrent = (node == ladder.getCurrent());
            bool isSafety = node->isSafetyLevel;

            Color boxColor = isCurrent ? colorAccent : 
                           isSafety ? {100, 150, 255, 200} : colorPrimary;

            DrawRectangleLines(startX, yPos, levelBoxWidth, levelBoxHeight, boxColor);
            DrawRectangle(startX + 2, yPos + 2, levelBoxWidth - 4, levelBoxHeight - 4, boxColor);

             string levelText = "Level " +  to_string(node->level);
             string prizeText = "Rs." + formatMoney(node->prizeAmount);

            DrawText(levelText.c_str(), startX + 5, yPos + 5, 10, colorText);
            DrawText(prizeText.c_str(), startX + 5, yPos + 18, 8, colorText);

            if (isSafety) {
                DrawText("SAFE", startX + 5, yPos + 28, 6, colorCorrect);
            }

            yPos += levelBoxHeight + spacing;
            node = node->next;
        }
    }

    void drawQuestionPanel(const Question& question, const Player& player) {
        int panelX = 160;
        int panelY = 50;
        int panelWidth = 750;
        int panelHeight = 700;

        // Draw panel background
        DrawRectangle(panelX, panelY, panelWidth, panelHeight, {30, 30, 50, 255});
        DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, colorPrimary);

        // Draw question
        int textX = panelX + 20;
        int textY = panelY + 20;

         string qHeader = "Question " +  to_string(question.id);
        DrawText(qHeader.c_str(), textX, textY, 16, colorAccent);

        // Draw question text (wrapped)
        drawWrappedText(question.text, textX, textY + 40, panelWidth - 40, 20, colorText);

        // Draw options
        int optionY = panelY + 200;
        for (int i = 0; i < 4; i++) {
            Color optionColor = colorPrimary;
            DrawRectangleLines(textX, optionY, panelWidth - 40, 60, optionColor);
            
             string optionText =  string(1, 'A' + i) + ") " + question.options[i];
            drawWrappedText(optionText, textX + 10, optionY + 15, panelWidth - 60, 16, colorText);

            optionY += 70;
        }

        // Draw hint
        int hintY = optionY + 20;
        DrawText("Hint:", textX, hintY, 14, colorAccent);
        drawWrappedText(question.hint, textX, hintY + 25, panelWidth - 40, 12, colorText);
    }

    void drawGameStatus(const Player& player) {
        int statusX = 920;
        int statusY = 50;
        int statusWidth = 250;

        DrawRectangle(statusX, statusY, statusWidth, 250, {30, 30, 50, 255});
        DrawRectangleLines(statusX, statusY, statusWidth, 250, colorAccent);

        int textX = statusX + 15;
        int textY = statusY + 15;
        int lineHeight = 30;

        DrawText("GAME INFO", textX, textY, 16, colorAccent);
        
        textY += lineHeight;
         string levelText = "Level: " +  to_string(player.currentLevel);
        DrawText(levelText.c_str(), textX, textY, 12, colorText);

        textY += lineHeight;
         string winText = "Winnings: Rs." + formatMoney(player.totalWinnings);
        DrawText(winText.c_str(), textX, textY, 12, colorCorrect);

        textY += lineHeight;
         string answeredText = "Answered: " +  to_string(player.questionsAnswered);
        DrawText(answeredText.c_str(), textX, textY, 12, colorText);

        textY += lineHeight;
         string lifelinesUsedText = "Lifelines Used: " + 
                                        to_string(player.lifelinesUsed[0] + 
                                                    player.lifelinesUsed[1] + 
                                                    player.lifelinesUsed[2]);
        DrawText(lifelinesUsedText.c_str(), textX, textY, 12, colorText);
    }

    void drawLifelines(const Player& player) {
        int lifelineX = 920;
        int lifelineY = 320;
        int lifelineWidth = 250;

        DrawRectangle(lifelineX, lifelineY, lifelineWidth, 150, {30, 30, 50, 255});
        DrawRectangleLines(lifelineX, lifelineY, lifelineWidth, 150, colorAccent);

        DrawText("LIFELINES", lifelineX + 15, lifelineY + 15, 14, colorAccent);

         vector< string> lifelineNames = {"50-50", "Ask Friend", "Skip"};
        int lifeY = lifelineY + 45;

        for (int i = 0; i < 3; i++) {
            Color btnColor = player.lifelinesUsed[i] ? Color{100, 100, 100, 255} : colorPrimary;
             string btnText = lifelineNames[i] + (player.lifelinesUsed[i] ? " (Used)" : "");
            
            DrawRectangle(lifelineX + 10, lifeY, lifelineWidth - 20, 25, btnColor);
            DrawText(btnText.c_str(), lifelineX + 15, lifeY + 5, 11, colorText);

            lifeY += 30;
        }
    }

    void drawTimer() {
        int timerX = 920;
        int timerY = 490;
        int timerWidth = 250;
        int timerHeight = 80;

        DrawRectangle(timerX, timerY, timerWidth, timerHeight, {30, 30, 50, 255});
        DrawRectangleLines(timerX, timerY, timerWidth, timerHeight, colorAccent);

        DrawText("TIME REMAINING", timerX + 15, timerY + 15, 12, colorAccent);
        DrawText("00:30", timerX + 50, timerY + 35, 32, colorWrong);
    }

    void drawGameOverScreen(const Player& player, long long finalWinnings) {
        drawCenteredText("GAME OVER", WINDOW_WIDTH / 2, 150, 48, colorWrong);

         string resultText = "You reached Level " +  to_string(player.currentLevel);
        drawCenteredText(resultText, WINDOW_WIDTH / 2, 250, 24, colorText);

         string winningsText = "Final Winnings: Rs. " + formatMoney(finalWinnings);
        drawCenteredText(winningsText, WINDOW_WIDTH / 2, 320, 28, colorCorrect);

         string statsText = "Questions Answered: " +  to_string(player.questionsAnswered);
        drawCenteredText(statsText, WINDOW_WIDTH / 2, 400, 18, colorText);

        drawButton("BACK TO MENU", WINDOW_WIDTH / 2 - 100, 550, 200, 50, colorPrimary);
    }

    void drawLeaderboardScreen(const Leaderboard& leaderboard) {
        drawCenteredText("LEADERBOARD", WINDOW_WIDTH / 2, 50, 40, colorAccent);

        int rankX = 200;
        int rankY = 150;
        int rankSpacing = 35;

        // Header
        DrawText("Rank", rankX, rankY, 14, colorAccent);
        DrawText("Name", rankX + 50, rankY, 14, colorAccent);
        DrawText("Winnings", rankX + 250, rankY, 14, colorAccent);
        DrawText("Level", rankX + 450, rankY, 14, colorAccent);

        rankY += 40;
        int rank = 1;

        const auto& topEntries = leaderboard.getTopEntries(10);
        for (const auto& entry : topEntries) {
             string rankStr =  to_string(rank);
            DrawText(rankStr.c_str(), rankX, rankY, 12, colorText);
            DrawText(entry.playerName.c_str(), rankX + 50, rankY, 12, colorText);
            
             string winningsStr = "Rs." + formatMoney(entry.winnings);
            DrawText(winningsStr.c_str(), rankX + 250, rankY, 12, colorCorrect);
            
             string levelStr =  to_string(entry.level);
            DrawText(levelStr.c_str(), rankX + 450, rankY, 12, colorText);

            rankY += rankSpacing;
            rank++;
        }

        drawButton("BACK", WINDOW_WIDTH / 2 - 50, 700, 100, 40, colorPrimary);
    }

private:
    Vector2 drawButton(const  string& text, int x, int y, int width, int height, Color color) {
        DrawRectangle(x, y, width, height, color);
        DrawRectangleLines(x, y, width, height, colorText);
        
        int textX = x + (width - MeasureText(text.c_str(), 20)) / 2;
        int textY = y + (height - 20) / 2;
        DrawText(text.c_str(), textX, textY, 20, colorText);

        return {(float)x, (float)y};
    }

    void drawCenteredText(const  string& text, int centerX, int y, int fontSize, Color color) {
        int textWidth = MeasureText(text.c_str(), fontSize);
        DrawText(text.c_str(), centerX - textWidth / 2, y, fontSize, color);
    }

    void drawWrappedText(const  string& text, int x, int y, int maxWidth, int fontSize, Color color) {
         string currentLine;
        int lineY = y;

        for (char c : text) {
            currentLine += c;
            int lineWidth = MeasureText(currentLine.c_str(), fontSize);

            if (lineWidth > maxWidth || c == '\n') {
                DrawText(currentLine.c_str(), x, lineY, fontSize, color);
                currentLine.clear();
                lineY += fontSize + 5;
            }
        }

        if (!currentLine.empty()) {
            DrawText(currentLine.c_str(), x, lineY, fontSize, color);
        }
    }

     string formatMoney(long long amount) {
        if (amount >= 10000000) {
            return  to_string(amount / 10000000) + "Cr";
        } else if (amount >= 100000) {
            return  to_string(amount / 100000) + "L";
        }
        return  to_string(amount);
    }
};

#endif // RAYLIB_ENABLED
#endif
