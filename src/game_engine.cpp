#include "game_engine.hpp"

using namespace std;

QuoteManager::QuoteManager()
{
    quotes = {
        "\"Efficiency isn't an option; it's the time complexity you choose.\"",
        "\"Graphs remind us that the world isn't linear.\"",
        "\"Linked lists link us to better understanding of memory.\"",
        "\"Trees grow complexity, but teach us recursion.\"",
        "\"Graphs are networks of possibilities waiting to be explored.\"",
        "\"Stack overflow? No, stack overflow!\"",
        "\"Queue up for success with proper data organization!\"",
        "\"Hash tables: Fast lookup, greater understanding!\"",
        "\"Sorting teaches patience; searching teaches efficiency.\"",
        "\"The beauty of DSA lies in solving the unsolvable elegantly.\""};
}

string QuoteManager::getRandomQuote() const
{
    if (quotes.empty())
        return "";
    return quotes[rand() % quotes.size()];
}

// --- GameEngine Implementation ---

GameEngine::GameEngine() : timeLimit(30), gameActive(false), correctAnswerStreak(0),
                           totalPointsEarned(0), currentCategoryId(-1)
{
}

bool GameEngine::initialize(const string &questionsFile)
{
    return questionBank.loadFromFile(questionsFile);
}

void GameEngine::setupPlayer(const string &name, const string &gender)
{
    player.name = name;
    player.gender = gender;
    playerProfileManager.getOrCreateProfile(name, gender);
    gameLogic.resetLifelines();
    correctAnswerStreak = 0;
    totalPointsEarned = 0;
    prizeLadder.resetLadder();
    player.totalWinnings = 0;
    player.currentLevel = 0;
    player.questionsAnswered = 0;
}

bool GameEngine::getNextQuestion()
{
    currentQuestion = questionBank.getNextQuestion(player);
    if (currentQuestion.id == -1)
        return false;
    player.recordQuestion(currentQuestion.id);

    currentCategoryId = currentQuestion.category;
    int difficulty = gameLogic.getNextDifficulty(player.currentLevel);
    timeLimit = gameLogic.getTimeLimit(difficulty);
    gameTimer.setDuration(timeLimit);
    gameTimer.start();

    return true;
}

bool GameEngine::processAnswer(int optionIndex)
{
    bool isCorrect = questionBank.isCorrectAnswer(currentQuestion.id, optionIndex);

    if (isCorrect)
    {
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
    }
    else
    {
        correctAnswerStreak = 0;
        if (!prizeLadder.isSafetyLevel())
        {
            prizeLadder.moveToSafetyLevel();
        }
        player.totalWinnings = prizeLadder.getCurrentPrize();
        return false;
    }
}

vector<int> GameEngine::use50_50Lifeline()
{
    if (!gameLogic.isLifelineAvailable(0))
        return {};

    player.lifelinesUsed[0] = 1;
    lifelineStack.usedLifeline(0);
    return gameLogic.apply50_50Lifeline(currentQuestion);
}

int GameEngine::useAskFriendLifeline()
{
    if (!gameLogic.isLifelineAvailable(1))
        return -1;

    player.lifelinesUsed[1] = 1;
    lifelineStack.usedLifeline(1);
    return gameLogic.applyAskFriendLifeline(currentQuestion);
}

bool GameEngine::useSkipLifeline()
{
    if (!gameLogic.isLifelineAvailable(2))
        return false;

    player.lifelinesUsed[2] = 1;
    lifelineStack.usedLifeline(2);
    return gameLogic.applySkipLifeline();
}

string GameEngine::useHintLifeline()
{
    if (!gameLogic.isLifelineAvailable(3))
        return "";

    player.lifelinesUsed[3] = 1;
    lifelineStack.usedLifeline(3);
    return gameLogic.applyHintLifeline(currentQuestion);
}

bool GameEngine::isLifelineAvailable(int lifelineType) const
{
    return gameLogic.isLifelineAvailable(lifelineType);
}

void GameEngine::endGame()
{
    gameActive = false;
    gameTimer.stop();

    playerProfileManager.updatePlayerStats(
        player.name,
        player.totalWinnings,
        player.currentLevel,
        player.questionsAnswered);

    leaderboard.addEntry(player, "");
}

string GameEngine::getRandomQuote() const
{
    return quoteManager.getRandomQuote();
}

const Player &GameEngine::getPlayer() const { return player; }
const PrizeLadder &GameEngine::getPrizeLadder() const { return prizeLadder; }
const Question &GameEngine::getCurrentQuestion() const { return currentQuestion; }
Leaderboard &GameEngine::getLeaderboard() { return leaderboard; }
int GameEngine::getTotalPoints() const { return totalPointsEarned; }
int GameEngine::getCorrectStreak() const { return correctAnswerStreak; }

PlayerStats GameEngine::getPlayerProfile()
{
    if (playerProfileManager.playerExists(player.name))
    {
        return playerProfileManager.getOrCreateProfile(player.name, player.gender);
    }
    return PlayerStats(player.name, player.gender);
}

PlayerProfileManager &GameEngine::getProfileManager() { return playerProfileManager; }

const GameTimer &GameEngine::getTimer() const { return gameTimer; }