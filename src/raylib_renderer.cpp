#include "raylib_renderer.hpp"
#include <cmath>
#include <algorithm>
#include <cctype>
#include <vector>
#include <cstring> -

static float measureTextSafe(Font font, const char *text, float fontSize, float spacing)
{
    if (font.texture.id == 0)
        return (float)MeasureText(text, (int)fontSize);
    Vector2 size = MeasureTextEx(font, text, fontSize, spacing);
    if (size.x < 1.0f && text[0] != '\0')
        return (float)strlen(text) * fontSize * 0.6f;
    return size.x;
}



void RaylibRenderer::drawCenteredText(const char *text, int y, int fontSize, Color color) const
{
    float textWidth = measureTextSafe(assets.gameFont, text, (float)fontSize, 1.0f);
    float x = ((float)1920 - textWidth) / 2.0f;
    DrawTextEx(assets.gameFont, text, {x, (float)y}, (float)fontSize, 1.0f, color);
}

void RaylibRenderer::drawTextEx(const char *text, float x, float y, float fontSize, Color color) const
{
    Vector2 pos = {x, y};
    DrawTextEx(assets.gameFont, text, pos, fontSize, 1.0f, color);
}

std::string RaylibRenderer::formatMoney(long long amount) const
{
    std::string s = std::to_string(amount);
    int insertPosition = s.length() - 3;
    while (insertPosition > 0)
    {
        s.insert(insertPosition, ",");
        insertPosition -= 3;
    }
    return "$" + s;
}

void RaylibRenderer::handleTextInput(char *buffer, int &count, int max_length, int key, bool active)
{
    if (!active) return;
    while (key > 0)
    {
        if ((key >= 32) && (key <= 125) && (count < max_length))
        {
            buffer[count] = (char)key;
            count++;
            buffer[count] = '\0';
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE))
    {
        if (count > 0) { count--; buffer[count] = '\0'; }
    }
}

Vector2 RaylibRenderer::getVirtualMousePosition()
{
    Vector2 mouse = GetMousePosition();
    Vector2 virtualMouse = {0};
    virtualMouse.x = (mouse.x - offset.x) / scale;
    virtualMouse.y = (mouse.y - offset.y) / scale;
    return virtualMouse;
}

string RaylibRenderer::getRandomResultPhrase(bool isCorrect)
{
    static const vector<string> correctPhrases = {
        "Bravo! That is correct!", "Excellent! You got it right.", "Spot on! Well done.", "That is the correct answer!", "Brilliant! Moving on."};
    static const vector<string> wrongPhrases = {
        "Oh no! That is incorrect.", "Tough luck! That's wrong.", "Incorrect! Better luck next time.", "Sorry, that is not the answer.", "Ouch! That's a wrong answer."};
    const vector<string> &targetList = isCorrect ? correctPhrases : wrongPhrases;
    return targetList[rand() % targetList.size()];
}

// --- Main Renderer Implementation ---

RaylibRenderer::RaylibRenderer(GameController &ctrl, GameEngine &eng)
    : controller(ctrl), engine(eng), nameBox(), genderBox()
{
    // FIX 1: Brute-force clear
    for (int i = 0; i < 5; i++)
    {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    target = LoadRenderTexture(virtualWidth, virtualHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    // FIX 2: Set startup fade
    startupAlpha = 1.0f;
    previousState = GameState::SPLASH; 

    // --- Asset Loading ---
    assets.gameFont = LoadFontEx("fonts/MonguleRegular-ov2Y0.ttf", 128, 0, 0);
    SetTextureFilter(assets.gameFont.texture, TEXTURE_FILTER_BILINEAR);

    assets.splashBg = LoadTexture("Graphics/splash.png");
    if (assets.splashBg.id == 0) assets.splashBg = LoadTexture("Graphics/splash_screen.png");

    assets.menuBg = LoadTexture("Graphics/menu_bg.png"); 
    if (assets.menuBg.id == 0) assets.menuBg = LoadTexture("Graphics/menu_bg.jpg");

    assets.setupBg = LoadTexture("Graphics/playerprofilebg.png");
    if (assets.setupBg.id == 0) assets.setupBg = LoadTexture("Graphics/playerprofilebg.jpg");

    assets.bgMaleZoomOut = LoadTexture("Graphics/ZoomoutM.png");
    assets.bgMaleZoomIn = LoadTexture("Graphics/ZoomMale.png");
    assets.bgFemaleZoomOut = LoadTexture("Graphics/ZoomoutFmale.png");
    assets.bgFemaleZoomIn = LoadTexture("Graphics/ZoominFemale.png");

    assets.bgGameOver = LoadTexture("Graphics/gameover.png");
    if (assets.bgGameOver.id == 0) assets.bgGameOver = LoadTexture("Graphics/gameover.jpg");

    // NEW: Load Win Background
    assets.bgWin = LoadTexture("Graphics/win.png");
    if (assets.bgWin.id == 0) assets.bgWin = LoadTexture("Graphics/win.jpg");

    assets.bgFinalScore = LoadTexture("Graphics/finalscore.png");
    if (assets.bgFinalScore.id == 0) assets.bgFinalScore = LoadTexture("Graphics/finalscore.jpg");

    assets.icon5050 = LoadTexture("Graphics/lifeline_5050.png");
    if (assets.icon5050.id == 0) assets.icon5050 = LoadTexture("Graphics/Play.png");

    assets.iconPhone = LoadTexture("Graphics/lifeline_phone.png");
    if (assets.iconPhone.id == 0) assets.iconPhone = LoadTexture("Graphics/Play.png");

    assets.iconSkip = LoadTexture("Graphics/lifeline_skip.png");
    if (assets.iconSkip.id == 0) assets.iconSkip = LoadTexture("Graphics/Play.png");

    assets.iconHint = LoadTexture("Graphics/lifeline_hint.png");
    if (assets.iconHint.id == 0) assets.iconHint = LoadTexture("Graphics/Play.png");

    // --- AUDIO LOADING ---
    assets.menuSound = LoadMusicStream("sounds/mainmenu.wav");
    PlayMusicStream(assets.menuSound);
    assets.menuSound.looping = true;

    // Sound 1: Timer Loop
    assets.musicTimer = LoadMusicStream("sounds/musicTimer.wav");
    assets.musicTimer.looping = true;

    // Sound 5: Leaderboard Loop
    assets.musicLeaderboard = LoadMusicStream("sounds/musicLeaderboard.wav");
    assets.musicLeaderboard.looping = true;

    // SFX
    assets.sfxCorrect = LoadSound("sounds/sfxCorrect.wav");
    assets.sfxWrong = LoadSound("sounds/sfxWrong.wav");
    assets.sfxGameOver = LoadSound("sounds/sfxGameOver.wav");
    assets.sfxWin = LoadSound("sounds/sfxWin.wav");
    assets.sfxLifeline = LoadSound("sounds/sfxLifeline.wav");

    float vWidth = (float)virtualWidth;
    float vHeight = (float)virtualHeight;

    // Menu Buttons
    assets.playButton = new Button("Graphics/Play.png", {vWidth * 0.25f, vHeight * 0.82f}, 0.3);
    assets.leaderButton = new Button("Graphics/leader.png", {vWidth * 0.45f, vHeight * 0.82f}, 0.3);
    assets.exitButton = new Button("Graphics/Exit.png", {vWidth * 0.65f, vHeight * 0.82f}, 0.3);

    float centerX = (vWidth - assets.leaderButton->GetWidth()) / 2.0f;
    assets.leaderButton->SetPosition({centerX, vHeight * 0.82f});
    assets.playButton->SetPosition({centerX - 350, vHeight * 0.82f});
    assets.exitButton->SetPosition({centerX + 350, vHeight * 0.82f});

    nameBox = {vWidth / 2 - 200, vHeight / 2 - 50, 400, 50};
    genderBox = {vWidth / 2 - 200, vHeight / 2 + 100, 400, 50};

    // Answer Buttons
    const char *optionImg = "Graphics/Play.png";
    float buttonScale = 0.3f;
    assets.optionButtons[0] = new Button(optionImg, {0, 0}, buttonScale);
    float realBtnWidth = assets.optionButtons[0]->GetWidth();
    float col1Center = vWidth * 0.25f;
    float col2Center = vWidth * 0.75f;
    float posX1 = col1Center - (realBtnWidth / 2.0f);
    float posX2 = col2Center - (realBtnWidth / 2.0f);
    float row1Y = vHeight * 0.60f;
    float row2Y = vHeight * 0.77f;

    assets.optionButtons[0]->SetPosition({posX1, row1Y});
    assets.optionButtons[1] = new Button(optionImg, {posX2, row1Y}, buttonScale);
    assets.optionButtons[2] = new Button(optionImg, {posX1, row2Y}, buttonScale);
    assets.optionButtons[3] = new Button(optionImg, {posX2, row2Y}, buttonScale);

    for (int i = 0; i < 4; i++) assets.optionButtons[i]->SetHitboxScale(0.85f, 0.75f);

    // Initialize Lifeline Rectangles
    float rectW = 100.0f;
    float rectH = 50.0f;
    float spacing = 20.0f;
    float totalW = 520.0f;
    float startX = vWidth - totalW - 50.0f;
    float startY = 40.0f;
    float currentX = startX;

    lifelineRects[0] = {currentX, startY, rectW, rectH}; currentX += rectW + spacing;
    float phoneWidth = 160.0f;
    lifelineRects[1] = {currentX, startY, phoneWidth, rectH}; currentX += phoneWidth + spacing;
    lifelineRects[2] = {currentX, startY, rectW, rectH}; currentX += rectW + spacing;
    lifelineRects[3] = {currentX, startY, rectW, rectH};
}

RaylibRenderer::~RaylibRenderer()
{
    UnloadRenderTexture(target);
    StopMusicStream(assets.menuSound);
    UnloadMusicStream(assets.menuSound);
    
    // Unload Audio
    UnloadMusicStream(assets.musicTimer);
    UnloadMusicStream(assets.musicLeaderboard);
    UnloadSound(assets.sfxCorrect);
    UnloadSound(assets.sfxWrong);
    UnloadSound(assets.sfxGameOver);
    UnloadSound(assets.sfxWin);
    UnloadSound(assets.sfxLifeline);

    UnloadFont(assets.gameFont);
    UnloadTexture(assets.splashBg);
    UnloadTexture(assets.setupBg);
    UnloadTexture(assets.menuBg);
    
    UnloadTexture(assets.bgMaleZoomOut);
    UnloadTexture(assets.bgMaleZoomIn);
    UnloadTexture(assets.bgFemaleZoomOut);
    UnloadTexture(assets.bgFemaleZoomIn);
    UnloadTexture(assets.bgGameOver);
    UnloadTexture(assets.bgWin); // Unload Win BG
    UnloadTexture(assets.bgFinalScore);
    UnloadTexture(assets.icon5050);
    UnloadTexture(assets.iconPhone);
    UnloadTexture(assets.iconSkip);
    UnloadTexture(assets.iconHint);
    
    delete assets.playButton;
    delete assets.leaderButton;
    delete assets.exitButton;
    for (int i = 0; i < 4; i++) delete assets.optionButtons[i];
}

void RaylibRenderer::drawAndHandleSplash()
{
    // UPDATED: No internal background drawing so updateAndDraw handles fullscreen scaling
    
    if (assets.splashBg.id == 0) {
        const char *message = "PRESS ENTER TO BEGIN";
        drawCenteredText(message, virtualHeight / 2, 60, RAYWHITE);
    }

    if ((int)(GetTime() * 2) % 2 == 0)
    {
        drawCenteredText("PRESS ENTER TO START", virtualHeight - 200, 40, WHITE);
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        controller.setState(GameState::MENU);
    }
}

void RaylibRenderer::drawAndHandleMenu(Vector2 mousePos, bool mousePressed)
{
    // UPDATED: No internal background drawing so updateAndDraw handles fullscreen scaling
    
    if (assets.menuBg.id == 0) // Fallback check if menu bg is missing, though we use splashBg now
    {
        // Only draw fallback text if neither background is loading correctly in updateAndDraw
    }

    if (startupAlpha < 1.0f) 
    {
        // Simple placeholder for consistency if you had fade logic here
    }

    float btnY = virtualHeight - 180; 
    float centerX = (virtualWidth - assets.leaderButton->GetWidth()) / 2;
    
    assets.leaderButton->SetPosition({centerX, btnY});
    assets.playButton->SetPosition({centerX - 350, btnY});
    assets.exitButton->SetPosition({centerX + 350, btnY});

    if (assets.playButton->isPressed(mousePos, mousePressed))
    {
        controller.setState(GameState::PLAYER_SETUP);
        playerNameBuffer[0] = '\0';
        playerGenderBuffer[0] = '\0';
        letterCountName = 0;
        letterCountGender = 0;
        activeTextBox = 1;
    }
    if (assets.leaderButton->isPressed(mousePos, mousePressed))
    {
            controller.setState(GameState::LEADERBOARD);
    }
    if (assets.exitButton->isPressed(mousePos, mousePressed))
    {
            controller.setState(GameState::EXIT);
    }

    assets.playButton->Draw(mousePos);
    assets.leaderButton->Draw(mousePos);
    assets.exitButton->Draw(mousePos);
}

void RaylibRenderer::drawAndHandlePlayerSetup(Vector2 mousePos, bool mousePressed, int key)
{
    if (mousePressed) {
        if (CheckCollisionPointRec(mousePos, nameBox)) activeTextBox = 1;
        else if (CheckCollisionPointRec(mousePos, genderBox)) activeTextBox = 2;
        else activeTextBox = 0;
    }
    SetMouseCursor(activeTextBox > 0 ? MOUSE_CURSOR_IBEAM : MOUSE_CURSOR_DEFAULT);

    handleTextInput(playerNameBuffer, letterCountName, MAX_NAME_LENGTH, key, activeTextBox == 1);
    if (activeTextBox == 2) {
        if (letterCountGender < 1) {
            if (key == 'm' || key == 'M' || key == 'f' || key == 'F') {
                playerGenderBuffer[0] = toupper((char)key);
                playerGenderBuffer[1] = '\0';
                letterCountGender = 1;
            }
        }
        if (IsKeyPressed(KEY_BACKSPACE) && letterCountGender > 0) {
            playerGenderBuffer[0] = '\0';
            letterCountGender = 0;
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (letterCountName > 0 && letterCountGender > 0) {
            std::string name(playerNameBuffer);
            std::string gender(playerGenderBuffer);
            controller.submitPlayerSetup(name, gender);
            activeTextBox = 0;
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
    }

    drawCenteredText("PLAYER SETUP", virtualHeight / 2 - 200, 40, YELLOW);
    drawTextEx("Enter Name:", nameBox.x, nameBox.y - 30, 20.0f, RAYWHITE);
    DrawRectangleRec(nameBox, LIGHTGRAY);
    DrawRectangleLines((int)nameBox.x, (int)nameBox.y, (int)nameBox.width, (int)nameBox.height, activeTextBox == 1 ? RED : DARKGRAY);
    drawTextEx(playerNameBuffer, nameBox.x + 5, nameBox.y + 10, 30.0f, MAROON);

    drawTextEx("Enter Gender (M/F):", genderBox.x, genderBox.y - 30, 20.0f, RAYWHITE);
    DrawRectangleRec(genderBox, LIGHTGRAY);
    DrawRectangleLines((int)genderBox.x, (int)genderBox.y, (int)genderBox.width, (int)genderBox.height, activeTextBox == 2 ? RED : DARKGRAY);
    drawTextEx(playerGenderBuffer, genderBox.x + 5, genderBox.y + 10, 30.0f, MAROON);

    if (activeTextBox == 1 && letterCountName < MAX_NAME_LENGTH && ((int)(GetTime() * 1.5) % 2 == 0)) {
        float textW = measureTextSafe(assets.gameFont, playerNameBuffer, 30.0f, 1.0f);
        drawTextEx("|", nameBox.x + 8 + textW, nameBox.y + 10, 30.0f, MAROON);
    } else if (activeTextBox == 2 && letterCountGender < 1 && ((int)(GetTime() * 1.5) % 2 == 0)) {
        float textW = measureTextSafe(assets.gameFont, playerGenderBuffer, 30.0f, 1.0f);
        drawTextEx("|", genderBox.x + 8 + textW, genderBox.y + 10, 30.0f, MAROON);
    }

    if (letterCountName > 0 && letterCountGender > 0) {
        drawTextEx("Press ENTER to Start Game", nameBox.x, genderBox.y + 70, 30.0f, GREEN);
    }
}

void RaylibRenderer::drawAndHandleGameIntro()
{
    drawCenteredText("READY TO BEGIN?", virtualHeight - 200, 50, RAYWHITE);
    drawCenteredText("Press ENTER to Start the Challenge", virtualHeight - 140, 30, YELLOW);
    if (IsKeyPressed(KEY_ENTER))
        controller.setState(GameState::QUESTION_DISPLAY);
}

void RaylibRenderer::drawTimer()
{
    const GameTimer &timer = engine.getTimer();
    int seconds = timer.getRemainingSeconds();

    Color timerColor = GREEN;
    if (seconds <= 10)
    {
         timerColor = ORANGE;
    }
    if (seconds <= 5)
    {
         timerColor = RED;
    }

    if (seconds <= 10 && seconds > 0) 
    {
        if (!IsMusicStreamPlaying(assets.musicTimer)) 
        {
            PlayMusicStream(assets.musicTimer);
        }
        UpdateMusicStream(assets.musicTimer);
    } 
    else 
    {
        StopMusicStream(assets.musicTimer);
    }

    float centerX = virtualWidth / 2.0f;
    float yPos = 80.0f;
    DrawCircle(centerX, yPos, 40, DARKGRAY);
    DrawCircleLines(centerX, yPos, 40, timerColor);
    string timeText = to_string(seconds);
    float textW = measureTextSafe(assets.gameFont, timeText.c_str(), 40.0f, 1.0f);
    drawTextEx(timeText.c_str(), centerX - textW / 2, yPos - 20, 40.0f, timerColor);
}

void RaylibRenderer::drawLifelinePopup()
{
    if (!controller.isLifelineMessageActive()) return;
    
    DrawRectangle(0, 0, virtualWidth, virtualHeight, Fade(BLACK, 0.7f));
    float boxW = 800; float boxH = 300;
    float boxX = (virtualWidth - boxW) / 2; float boxY = (virtualHeight - boxH) / 2;
    DrawRectangleRounded({boxX, boxY, boxW, boxH}, 0.1f, 10, DARKBLUE);
    DrawRectangleLinesEx({boxX, boxY, boxW, boxH}, 3.0f, GOLD);
    drawCenteredText("LIFELINE USED", boxY + 40, 40, GOLD);
    string msg = controller.getLifelineMessage();
    drawCenteredText(msg.c_str(), boxY + 120, 30, WHITE);
    drawCenteredText("Press ENTER to Close", boxY + 220, 25, LIGHTGRAY);
    if (IsKeyPressed(KEY_ENTER)) 
    {
        controller.closeLifelineMessage();
    }
}

void RaylibRenderer::drawAndHandleGameplay(Vector2 mousePos, bool mousePressed)
{
    const Question &q = engine.getCurrentQuestion();
    if (q.options.size() < 4) 
    {
        drawCenteredText("Loading Question...", 200, 30, WHITE);
        return;
    }

    drawTimer();
    const char *labels[] = {"50:50", "Phone a Friend", "Skip", "Hint"};
    bool interactionEnabled = !controller.isLifelineMessageActive();

    for (int i = 0; i < 4; i++)
    {
        bool available = engine.isLifelineAvailable(i);
        Color rectColor = available ? SKYBLUE : DARKGRAY;
        Color borderColor = available ? BLUE : BLACK;
        bool hover = false;
        if (available && interactionEnabled)
        {
            hover = CheckCollisionPointRec(mousePos, lifelineRects[i]);
            if (hover)
            {
                 rectColor = BLUE;
            }
            if (hover && mousePressed) 
            {
                if (i == 1) 
                {
                    PlaySound(assets.sfxLifeline); 
                }
                controller.handleLifelineUsage(i);
            }
        }
        DrawRectangleRounded(lifelineRects[i], 0.3f, 5, rectColor);
        DrawRectangleLinesEx(lifelineRects[i], 2.0f, borderColor);
        float txtW = measureTextSafe(assets.gameFont, labels[i], 20.0f, 1.0f);
        drawTextEx(labels[i], lifelineRects[i].x + (lifelineRects[i].width - txtW) / 2, lifelineRects[i].y + 15, 20.0f, BLACK);
    }

    if (controller.isLifelineMessageActive()) 
    {
        drawLifelinePopup();
        return;
    }

    Color navyBlue = {0, 0, 128, 255};
    Color navyBlueHover = {30, 30, 160, 255};
    Color boxBorder = GOLD;
    float vWidth = (float)virtualWidth; float vHeight = (float)virtualHeight;
    float qBoxWidth = vWidth * 0.9f; float qBoxHeight = 150.0f;
    float qBoxX = (vWidth - qBoxWidth) / 2.0f; float qBoxY = vHeight * 0.55f;
    Rectangle qBoxRect = {qBoxX, qBoxY, qBoxWidth, qBoxHeight};
    DrawRectangleRounded(qBoxRect, 0.2f, 10, navyBlue);
    DrawRectangleLinesEx(qBoxRect, 3.0f, boxBorder);
    float qTextW = measureTextSafe(assets.gameFont, q.text.c_str(), 35.0f, 1.0f);
    drawTextEx(q.text.c_str(), qBoxX + (qBoxWidth - qTextW) / 2, qBoxY + (qBoxHeight / 2) - 17, 35.0f, WHITE);

    float optWidth = (vWidth * 0.4f); float optHeight = 80.0f;
    float gapX = vWidth * 0.05f; float gapY = 20.0f;
    float startX_Col1 = (vWidth - (optWidth * 2 + gapX)) / 2.0f;
    float startX_Col2 = startX_Col1 + optWidth + gapX;
    float startY_Row1 = qBoxY + qBoxHeight + 30.0f;
    float startY_Row2 = startY_Row1 + optHeight + gapY;
    Rectangle optRects[4] = {
        {startX_Col1, startY_Row1, optWidth, optHeight},
        {startX_Col2, startY_Row1, optWidth, optHeight},
        {startX_Col1, startY_Row2, optWidth, optHeight},
        {startX_Col2, startY_Row2, optWidth, optHeight}};

    const vector<int> &hidden = controller.getHiddenOptions();
    for (int i = 0; i < 4; i++)
    {
        bool isHidden = false;
        for (int h : hidden) { if (h == i) isHidden = true; }
        if (isHidden) {
            DrawRectangleRounded(optRects[i], 0.3f, 6, Fade(DARKGRAY, 0.5f));
            continue;
        }
        bool isHover = interactionEnabled && CheckCollisionPointRec(mousePos, optRects[i]);
        Color drawColor = isHover ? navyBlueHover : navyBlue;
        DrawRectangleRounded(optRects[i], 0.3f, 6, drawColor);
        DrawRectangleLinesEx(optRects[i], 2.0f, boxBorder);
        if (isHover && mousePressed) 
        {
            controller.submitAnswer(i);
        }
        string optText = ""; char prefix = 'A' + i; optText += prefix; optText += ": "; optText += q.options[i];
        float optW = measureTextSafe(assets.gameFont, optText.c_str(), 30.0f, 1.0f);
        drawTextEx(optText.c_str(), optRects[i].x + (optWidth - optW) / 2, optRects[i].y + (optHeight / 2) - 15, 30.0f, WHITE);
    }
}

void RaylibRenderer::drawPrizeLadder()
{
    const PrizeLadder &ladder = engine.getPrizeLadder();
    const Player &player = engine.getPlayer();
    int currentLevel = player.currentLevel;
    float hdrW = 700.0f; float hdrH = 100.0f;
    float hdrX = (virtualWidth - hdrW) / 2.0f; float hdrY = 30.0f;
    Rectangle hdrRect = {hdrX, hdrY, hdrW, hdrH};
    DrawRectangleRounded(hdrRect, 0.2f, 10, {0, 0, 40, 255});
    DrawRectangleLinesEx(hdrRect, 4.0f, GOLD);
    drawCenteredText("PRIZE LADDER", 50, 60, GOLD);
    float boxWidth = 500.0f; float boxHeight = 50.0f; float spacing = 10.0f;
    float startX = (virtualWidth - boxWidth) / 2.0f; float bottomY = virtualHeight - 100.0f;
    PrizeNode *curr = ladder.getHead();
    vector<PrizeNode *> levels;
    while (curr) {
        if (curr->level > 0) levels.push_back(curr);
        curr = curr->next;
    }
    for (const auto &node : levels) {
        int lvl = node->level; long long prize = node->prizeAmount; bool isSafety = node->isSafetyLevel;
        float yPos = bottomY - (lvl - 1) * (boxHeight + spacing);
        Rectangle rect = {startX, yPos, boxWidth, boxHeight};
        Color fillColor = DARKGRAY; Color textColor = WHITE; Color borderColor = WHITE;
        if (lvl <= currentLevel) fillColor = GREEN;
        else if (lvl == currentLevel + 1) { fillColor = ORANGE; textColor = BLACK; }
        else fillColor = {0, 0, 80, 255};
        if (isSafety) borderColor = GOLD;
        DrawRectangleRounded(rect, 0.3f, 6, fillColor);
        DrawRectangleLinesEx(rect, 3.0f, borderColor);
        string label = to_string(lvl) + "   $" + formatMoney(prize);
        float lblW = measureTextSafe(assets.gameFont, label.c_str(), 30.0f, 1.0f);
        drawTextEx(label.c_str(), rect.x + (boxWidth - lblW) / 2, rect.y + (boxHeight / 2) - 15, 30.0f, textColor);
    }
    drawCenteredText("Press ENTER to Continue", virtualHeight - 50, 30, WHITE);
    if (IsKeyPressed(KEY_ENTER)) controller.clearPause();
}

void RaylibRenderer::drawLeaderboard()
{
    Color COL_BG_DARK = {15, 20, 30, 255};
    Color COL_BG_GRADIENT = {25, 30, 50, 255};
    Color COL_CARD_BG = {40, 44, 70, 255};
    Color COL_ROW_BG = {30, 35, 50, 255};
    Color COL_ROW_HOVER = {50, 55, 80, 255};
    Color COL_GOLD = {255, 200, 0, 255};
    Color COL_SILVER = {200, 200, 200, 255};
    Color COL_BRONZE = {205, 127, 50, 255};
    Color COL_TEXT_HINT = {150, 160, 180, 255};

    DrawRectangle(0, 0, virtualWidth, virtualHeight, COL_BG_DARK);
    DrawRectangleGradientV(0, 0, virtualWidth, virtualHeight, COL_BG_DARK, COL_BG_GRADIENT);

    const char *title = "HALL OF FAME";
    drawCenteredText(title, 50, 80, WHITE);
    float titleWidth = measureTextSafe(assets.gameFont, title, 80, 1.0f);
    DrawRectangle(virtualWidth / 2 - (titleWidth / 2), 140, titleWidth, 4, COL_GOLD);

    Vector2 mouse = getVirtualMousePosition();
    const auto &entries = engine.getLeaderboard().getTopEntries(8);

    if (entries.empty())
    {
        drawCenteredText("No Champions Yet... Be the First!", virtualHeight / 2, 40, COL_TEXT_HINT);
        if (IsKeyPressed(KEY_ENTER)) controller.clearPause();
        return;
    }

    auto drawCard = [&](const LeaderboardEntry &p, int rank, int x, int y, float baseScale)
    {
        float baseW = 300.0f; float baseH = 380.0f;
        Rectangle hitRect = {(float)x, (float)y, baseW * baseScale, baseH * baseScale};
        bool isHover = CheckCollisionPointRec(mouse, hitRect);
        float finalScale = isHover ? baseScale * 1.05f : baseScale;
        float currentW = baseW * finalScale; float currentH = baseH * finalScale;
        float offsetX = (currentW - (baseW * baseScale)) / 2.0f; float offsetY = (currentH - (baseH * baseScale)) / 2.0f;
        float drawX = x - offsetX; float drawY = y - offsetY;
        Color accentColor = (rank == 1) ? COL_GOLD : ((rank == 2) ? COL_SILVER : COL_BRONZE);
        if (isHover) accentColor = WHITE;
        Rectangle cardRect = {drawX, drawY, currentW, currentH};
        DrawRectangleRounded(cardRect, 0.1f, 10, COL_CARD_BG);
        DrawRectangleLinesEx(cardRect, 3.0f, accentColor);
        int cx = drawX + currentW / 2; int cy = drawY + 60 * finalScale; int radius = 40 * finalScale;
        DrawCircle(cx, cy, radius, accentColor); DrawCircle(cx, cy, radius - 3, COL_BG_DARK);
        char initial[2] = {(char)toupper(p.playerName[0]), '\0'};
        int initialSize = 50 * finalScale;
        float initialW = measureTextSafe(assets.gameFont, initial, (float)initialSize, 1.0f);
        drawTextEx(initial, cx - initialW / 2, cy - initialSize / 2, (float)initialSize, WHITE);
        if (rank == 1) drawTextEx("W", cx - 10, drawY + 10, 20, COL_GOLD);
        int nameSize = 28 * finalScale;
        string nameDisplay = p.playerName;
        if (nameDisplay.length() > 12) nameDisplay = nameDisplay.substr(0, 10) + "..";
        float nameW = measureTextSafe(assets.gameFont, nameDisplay.c_str(), (float)nameSize, 1.0f);
        drawTextEx(nameDisplay.c_str(), cx - nameW / 2, cy + radius + 15, (float)nameSize, WHITE);
        string prize = formatMoney(p.winnings);
        int prizeSize = 32 * finalScale;
        float prizeW = measureTextSafe(assets.gameFont, prize.c_str(), (float)prizeSize, 1.0f);
        drawTextEx(prize.c_str(), cx - prizeW / 2, cy + radius + 50, (float)prizeSize, COL_GOLD);
        DrawLine(drawX + 20, drawY + currentH - 90, drawX + currentW - 20, drawY + currentH - 90, COL_TEXT_HINT);
        string lvl = "Lvl " + to_string(p.level);
        string qst = to_string(p.gamesPlayed) + " Qs";
        drawTextEx(lvl.c_str(), drawX + 30, drawY + currentH - 50, 24, COL_TEXT_HINT);
        float qstW = measureTextSafe(assets.gameFont, qst.c_str(), 24, 1.0f);
        drawTextEx(qst.c_str(), drawX + currentW - 30 - qstW, drawY + currentH - 50, 24, COL_TEXT_HINT);
    };

    int centerX = virtualWidth / 2; int cardY = 180; int cardBaseWidth = 300; int cardGap = 40;
    int r1W = cardBaseWidth; int r1X = centerX - (r1W / 2);
    int r2W = cardBaseWidth * 0.9f; int r2X = r1X - cardGap - r2W;
    int r3W = cardBaseWidth * 0.9f; int r3X = r1X + r1W + cardGap;

    if (entries.size() > 1) drawCard(entries[1], 2, r2X, cardY + 40, 0.9f);
    if (entries.size() > 2) drawCard(entries[2], 3, r3X, cardY + 40, 0.9f);
    if (entries.size() > 0) drawCard(entries[0], 1, r1X, cardY, 1.0f);

    int headerY = 580; int listY = headerY + 50; int rowHeight = 60; int rowW = 1200;
    int listX = (virtualWidth - rowW) / 2;
    int colRank = listX + 20; int colPlayer = listX + (rowW * 0.08f) + 20;
    int colPrize = listX + (rowW * 0.40f); int colLevel = listX + (rowW * 0.60f);
    int colQuestStart = listX + (rowW * 0.75f); int colQuestWidth = (rowW * 0.25f);
    int colQuestCenter = colQuestStart + (colQuestWidth / 2);

    if (entries.size() > 3)
    {
        Color headerCol = COL_GOLD; int headerSz = 32;
        drawTextEx("RANK", colRank, headerY, headerSz, headerCol);
        drawTextEx("PLAYER", colPlayer, headerY, headerSz, headerCol);
        drawTextEx("PRIZE", colPrize, headerY, headerSz, headerCol);
        drawTextEx("LEVEL", colLevel, headerY, headerSz, headerCol);
        const char *qHeader = "Questions Attempted";
        float qHeaderW = measureTextSafe(assets.gameFont, qHeader, headerSz, 1.0f);
        drawTextEx(qHeader, colQuestCenter - (qHeaderW / 2), headerY, headerSz, headerCol);
    }

    for (size_t i = 3; i < entries.size(); i++)
    {
        const auto &e = entries[i];
        Rectangle rowRect = {(float)listX, (float)listY, (float)rowW, (float)rowHeight};
        bool isRowHover = CheckCollisionPointRec(mouse, rowRect);
        DrawRectangleRounded(rowRect, 0.2f, 8, isRowHover ? COL_ROW_HOVER : COL_ROW_BG);
        if (isRowHover) DrawRectangle(listX + 5, listY + 10, 4, rowHeight - 20, COL_GOLD);
        string rankStr = "#" + to_string(i + 1);
        drawTextEx(rankStr.c_str(), colRank, listY + 15, 28, COL_TEXT_HINT);
        drawTextEx(e.playerName.c_str(), colPlayer, listY + 15, 28, WHITE);
        drawTextEx(formatMoney(e.winnings).c_str(), colPrize, listY + 15, 28, COL_GOLD);
        drawTextEx(to_string(e.level).c_str(), colLevel + 20, listY + 15, 28, WHITE);
        string qStr = to_string(e.gamesPlayed);
        float qStrW = measureTextSafe(assets.gameFont, qStr.c_str(), 28, 1.0f);
        drawTextEx(qStr.c_str(), colQuestCenter - (qStrW / 2), listY + 15, 28, WHITE);
        listY += (rowHeight + 10);
    }

    if ((int)(GetTime() * 1.5) % 2 == 0)
        drawCenteredText("PRESS [ENTER] TO RETURN TO MENU", virtualHeight - 30, 30, WHITE);
    if (IsKeyPressed(KEY_ENTER)) controller.clearPause();
}

void RaylibRenderer::drawResultDisplay()
{
    bool isCorrect = (engine.getCorrectStreak() > 0);
    if (cachedResultText.empty()) cachedResultText = getRandomResultPhrase(isCorrect);
    
    Color navyBlue = {0, 0, 128, 255}; Color boxBorder = GOLD;
    float vWidth = (float)virtualWidth; float vHeight = (float)virtualHeight;
    float boxWidth = vWidth * 0.8f; float boxHeight = isCorrect ? 250.0f : 400.0f;
    float boxX = (vWidth - boxWidth) / 2.0f; float boxY = (vHeight - boxHeight) / 2.0f;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.2f, 10, navyBlue);
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3.0f, boxBorder);
    drawCenteredText(cachedResultText.c_str(), boxY + 40, 50, WHITE);

    if (!isCorrect)
    {
        drawCenteredText("Better luck next time!", boxY + 120, 30, YELLOW);
        const Question &q = engine.getCurrentQuestion();
        if (q.correctAnswerIndex >= 0 && q.correctAnswerIndex < q.options.size())
        {
            string correctAnswerText = "The correct answer was: " + q.options[q.correctAnswerIndex];
            drawCenteredText(correctAnswerText.c_str(), boxY + 220, 30, GREEN);
        }
    }
    drawCenteredText("Press ENTER to Continue", boxY + (isCorrect ? 180 : 320), 30, LIGHTGRAY);
    if (IsKeyPressed(KEY_ENTER)) { cachedResultText.clear(); controller.clearPause(); }
}

void RaylibRenderer::drawGameOverScreen()
{
    drawCenteredText("Press ENTER to Continue", virtualHeight - 100, 30, WHITE);
    if (IsKeyPressed(KEY_ENTER)) controller.clearPause();
}

void RaylibRenderer::drawAndHandleFinalScore()
{
    const Player &p = engine.getPlayer();
    string winningsText = "Total Winnings: " + formatMoney(p.totalWinnings);
    string levelText = "Final Level Reached: " + to_string(p.currentLevel);
    string correctText = "Questions answered correctly: " + to_string(p.questionsAnswered);
    int centerY = virtualHeight / 2; int spacing = 60;

    drawCenteredText("FINAL RESULTS", centerY - 100, 60, GOLD);
    drawCenteredText(winningsText.c_str(), centerY, 50, BLACK);
    drawCenteredText(levelText.c_str(), centerY + spacing, 50, BLACK);
    drawCenteredText(correctText.c_str(), centerY + spacing * 2, 50, BLACK);

    if (cachedQuote.empty()) cachedQuote = engine.getRandomQuote();
    drawCenteredText(cachedQuote.c_str(), centerY + spacing * 3 + 20, 25, DARKGRAY);

    float btnWidth = 300; float btnHeight = 60; float gap = 30;
    float startX = (virtualWidth - (btnWidth * 4) - (gap * 3)) / 2; float btnY = virtualHeight - 100;
    Rectangle replayBtnRect = {startX, btnY, btnWidth, btnHeight};
    Rectangle menuBtnRect = {startX + btnWidth + gap, btnY, btnWidth, btnHeight};
    Rectangle leaderBtnRect = {startX + (btnWidth + gap) * 2, btnY, btnWidth, btnHeight};
    Rectangle exitBtnRect = {startX + (btnWidth + gap) * 3, btnY, btnWidth, btnHeight};

    Vector2 mouse = getVirtualMousePosition();
    bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    Color btnColorNormal = SKYBLUE; Color btnColorHover = BLUE; Color textColor = RAYWHITE;

    // REPLAY
    bool hoverReplay = CheckCollisionPointRec(mouse, replayBtnRect);
    DrawRectangleRec(replayBtnRect, hoverReplay ? btnColorHover : btnColorNormal);
    DrawRectangleLinesEx(replayBtnRect, 2, DARKBLUE);
    Vector2 replayTextSize = MeasureTextEx(assets.gameFont, "REPLAY", 30.0f, 1.0f);
    drawTextEx("REPLAY", replayBtnRect.x + (btnWidth - replayTextSize.x) / 2, replayBtnRect.y + (btnHeight - replayTextSize.y) / 2, 30.0f, textColor);
    if (hoverReplay && clicked) { cachedQuote.clear(); string name = p.name; string gender = p.gender; controller.setState(GameState::PLAYER_SETUP); controller.submitPlayerSetup(name, gender); }

    // MAIN MENU
    bool hoverMenu = CheckCollisionPointRec(mouse, menuBtnRect);
    DrawRectangleRec(menuBtnRect, hoverMenu ? btnColorHover : btnColorNormal);
    DrawRectangleLinesEx(menuBtnRect, 2, DARKBLUE);
    Vector2 menuTextSize = MeasureTextEx(assets.gameFont, "MAIN MENU", 30.0f, 1.0f);
    drawTextEx("MAIN MENU", menuBtnRect.x + (btnWidth - menuTextSize.x) / 2, menuBtnRect.y + (btnHeight - menuTextSize.y) / 2, 30.0f, textColor);
    if (hoverMenu && clicked) { cachedQuote.clear(); controller.setState(GameState::MENU); }

    // LEADERBOARD
    bool hoverLeader = CheckCollisionPointRec(mouse, leaderBtnRect);
    DrawRectangleRec(leaderBtnRect, hoverLeader ? btnColorHover : btnColorNormal);
    DrawRectangleLinesEx(leaderBtnRect, 2, DARKBLUE);
    Vector2 leadTextSize = MeasureTextEx(assets.gameFont, "LEADERBOARD", 30.0f, 1.0f);
    drawTextEx("LEADERBOARD", leaderBtnRect.x + (btnWidth - leadTextSize.x) / 2, leaderBtnRect.y + (btnHeight - leadTextSize.y) / 2, 30.0f, textColor);
    if (hoverLeader && clicked) { cachedQuote.clear(); controller.setState(GameState::LEADERBOARD); }

    // EXIT
    bool hoverExit = CheckCollisionPointRec(mouse, exitBtnRect);
    DrawRectangleRec(exitBtnRect, hoverExit ? RED : MAROON);
    DrawRectangleLinesEx(exitBtnRect, 2, DARKGRAY);
    Vector2 exitTextSize = MeasureTextEx(assets.gameFont, "EXIT", 30.0f, 1.0f);
    drawTextEx("EXIT", exitBtnRect.x + (btnWidth - exitTextSize.x) / 2, exitBtnRect.y + (btnHeight - exitTextSize.y) / 2, 30.0f, textColor);
    if (hoverExit && clicked) controller.setState(GameState::EXIT);
}

// --- NEW: Helper to manage one-shot sounds and loop transitions ---
void RaylibRenderer::handleStateAudio(GameState currentState)
{
    if (currentState != previousState) 
    {
        // Stop loops from previous states
        StopMusicStream(assets.musicLeaderboard);
        StopMusicStream(assets.musicTimer); 

        if (currentState == GameState::GAME_OVER) 
        {
            // FIX: Stop "Wrong" sound so Game Over sound is clear
            StopSound(assets.sfxWrong);
            StopSound(assets.sfxCorrect);

            if (engine.getPlayer().currentLevel == 15) // CHECK IF WIN
            {
                PlaySound(assets.sfxWin);
            }
            else
            {
                PlaySound(assets.sfxGameOver); 
            }
        }
        
       

        if (currentState == GameState::LEADERBOARD) 
        {
            PlayMusicStream(assets.musicLeaderboard); 
        }

        if (currentState == GameState::RESULT_DISPLAY) 
        {
            if (engine.getCorrectStreak() > 0) 
            {
                PlaySound(assets.sfxCorrect); 
            }
            else 
            {
                PlaySound(assets.sfxWrong);   
            }
        }
        previousState = currentState;
    }
    if (currentState == GameState::LEADERBOARD) 
    {
        UpdateMusicStream(assets.musicLeaderboard);
    }
}

void RaylibRenderer::updateAndDraw()
{
    float scaleX = (float)GetScreenWidth() / virtualWidth;
    float scaleY = (float)GetScreenHeight() / virtualHeight;
    scale = min(scaleX, scaleY);
    offset.x = (GetScreenWidth() - (virtualWidth * scale)) * 0.5f;
    offset.y = (GetScreenHeight() - (virtualHeight * scale)) * 0.5f;
    Vector2 virtualMousePos = getVirtualMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    int key = GetCharPressed();
    GameState state = controller.getState();

    handleStateAudio(state);

    if (startupAlpha > 0.0f) {
        startupAlpha -= GetFrameTime() * 1.5f; 
        if (startupAlpha < 0.0f) startupAlpha = 0.0f;
    }
    if (state != GameState::FINAL_SCORE && !cachedQuote.empty()) cachedQuote.clear();
    if (state != GameState::RESULT_DISPLAY && !cachedResultText.empty()) cachedResultText.clear();

    bool shouldPlayMusic = (state == GameState::SPLASH ||
                            state == GameState::MENU ||
                            state == GameState::GAME_INTRO);

    if (shouldPlayMusic) {
        if (!IsMusicStreamPlaying(assets.menuSound)) PlayMusicStream(assets.menuSound);
        UpdateMusicStream(assets.menuSound);
    } else {
        if (IsMusicStreamPlaying(assets.menuSound)) StopMusicStream(assets.menuSound);
    }

    BeginTextureMode(target);
    ClearBackground({0, 0, 0, 0});

    switch (state)
    {
    case GameState::SPLASH: drawAndHandleSplash(); break;
    case GameState::MENU: drawAndHandleMenu(virtualMousePos, mousePressed); break;
    case GameState::PLAYER_SETUP: drawAndHandlePlayerSetup(virtualMousePos, mousePressed, key); break;
    case GameState::GAME_INTRO: drawAndHandleGameIntro(); break;
    case GameState::QUESTION_DISPLAY:
    case GameState::ANSWER_PROCESSING:
        controller.update();
        drawAndHandleGameplay(virtualMousePos, mousePressed);
        break;
    case GameState::RESULT_DISPLAY: drawResultDisplay(); break;
    case GameState::PRIZE_LADDER: drawPrizeLadder(); break;
    case GameState::GAME_OVER: drawGameOverScreen(); break;
    case GameState::FINAL_SCORE: drawAndHandleFinalScore(); break;
    case GameState::LEADERBOARD: drawLeaderboard(); break;
    case GameState::EXIT: break;
    default: drawCenteredText("UNHANDLED STATE ERROR", virtualHeight / 2, 50, MAGENTA);
    }
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    if (state == GameState::LEADERBOARD) {
        Color COL_BG_DARK = {15, 20, 30, 255};
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), COL_BG_DARK);
    }

    if (state != GameState::LEADERBOARD) { 
        Texture2D *bgToDraw = &assets.splashBg;
        Color tint = WHITE;

        if (state == GameState::SPLASH) { tint = GRAY; }
        else if (state == GameState::MENU) {
            if (assets.menuBg.id != 0) bgToDraw = &assets.menuBg;
            else tint = LIGHTGRAY;
        }
        else if (state == GameState::PLAYER_SETUP) {
            if (assets.setupBg.id != 0) bgToDraw = &assets.setupBg;
            else tint = LIGHTGRAY;
        }
        else if (state == GameState::GAME_OVER) {
            // CHECK IF WIN
            if (engine.getPlayer().currentLevel == 15) {
                if (assets.bgWin.id != 0) bgToDraw = &assets.bgWin;
            } else {
                if (assets.bgGameOver.id != 0) bgToDraw = &assets.bgGameOver;
            }
        }
        else if (state == GameState::FINAL_SCORE) {
            if (assets.bgFinalScore.id != 0) bgToDraw = &assets.bgFinalScore;
        }
        else if (state == GameState::GAME_INTRO) {
            string gender = engine.getPlayer().gender;
            if (gender == "M" || gender == "m") { if (assets.bgMaleZoomOut.id != 0) bgToDraw = &assets.bgMaleZoomOut; }
            else { if (assets.bgFemaleZoomOut.id != 0) bgToDraw = &assets.bgFemaleZoomOut; }
        }
        else if (state == GameState::QUESTION_DISPLAY || state == GameState::ANSWER_PROCESSING || state == GameState::RESULT_DISPLAY || state == GameState::PRIZE_LADDER) {
            string gender = engine.getPlayer().gender;
            if (gender == "M" || gender == "m") { if (assets.bgMaleZoomIn.id != 0) bgToDraw = &assets.bgMaleZoomIn; }
            else { if (assets.bgFemaleZoomIn.id != 0) bgToDraw = &assets.bgFemaleZoomIn; }
        }

        // --- DRAW BACKGROUND SCALED TO FULL SCREEN ---
        Rectangle source = {0.0f, 0.0f, (float)bgToDraw->width, (float)bgToDraw->height};
        Rectangle dest = {0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight()};
        DrawTexturePro(*bgToDraw, source, dest, {0, 0}, 0.0f, tint);
    }

    Rectangle sourceRec = {0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height};
    Rectangle destRec = {offset.x, offset.y, (float)virtualWidth * scale, (float)virtualHeight * scale};
    DrawTexturePro(target.texture, sourceRec, destRec, {0, 0}, 0.0f, WHITE);

    if (startupAlpha > 0.0f) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, startupAlpha));
    }
    EndDrawing();
}