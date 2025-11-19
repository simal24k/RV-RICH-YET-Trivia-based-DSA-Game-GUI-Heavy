#include "game_engine.hpp"
#include <ctime>

using namespace std;

QuoteManager::QuoteManager() {
    quotes = {
        "\"A well-structured data structure is the foundation of efficient algorithms!\" - Sir Donald Knuth",
        "\"In DSA we trust: Divide, conquer, and optimize!\" - The Algorithm Philosophy",
        "\"Linked lists link us to better understanding of memory.\" - Computer Science",
        "\"Trees grow complexity, but teach us recursion.\" - DSA Wisdom",
        "\"Graphs are networks of possibilities waiting to be explored.\" - Network Theory",
        "\"Stack overflow? No, stack overflow!\" - Debugging Moments",
        "\"Queue up for success with proper data organization!\" - FIFO Logic",
        "\"Hash tables: Fast lookup, greater understanding!\" - Performance Optimization",
        "\"Sorting teaches patience; searching teaches efficiency.\" - Algorithm Lessons",
        "\"The beauty of DSA lies in solving the unsolvable elegantly.\" - Software Engineering"
    };
}

string QuoteManager::getRandomQuote() const {
    if (quotes.empty()) return "";
    return quotes[rand() % quotes.size()];
}

void QuoteManager::displayRandomQuote() const {
    cout << "\n╔════════════════════════════════════════════════════╗\n";
    cout << "║          DSA QUOTE OF THE MOMENT                  ║\n";
    cout << "║════════════════════════════════════════════════════║\n";
    cout << "║ " << getRandomQuote() << " ║\n";
    cout << "╚════════════════════════════════════════════════════╝\n";
}

GameEngine::GameEngine() : timeLimit(30), gameActive(false), correctAnswerStreak(0), 
                          totalPointsEarned(0), showGameStatus(false) {}

bool GameEngine::initialize(const string& questionsFile) {
    return questionBank.loadFromFile(questionsFile);
}

void GameEngine::setupPlayer(const string& name, const string& gender) {
    player.name = name;
    player.gender = gender;
    playerProfileManager.getOrCreateProfile(name, gender);
    gameLogic.resetLifelines();
    correctAnswerStreak = 0;
    totalPointsEarned = 0;
    showGameStatus = true; // Show status after setup
}

bool GameEngine::getNextQuestion() {
    currentQuestion = questionBank.getNextQuestion(player);
    if (currentQuestion.id == -1) return false;
    player.recordQuestion(currentQuestion.id);
    
    int difficulty = gameLogic.getNextDifficulty(player.currentLevel);
    timeLimit = gameLogic.getTimeLimit(difficulty);
    gameTimer.setDuration(timeLimit);
    gameTimer.start();
    
    return true;
}

bool GameEngine::processAnswer(int optionIndex) {
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

vector<int> GameEngine::use50_50Lifeline() {
    if (!gameLogic.isLifelineAvailable(0)) return {};
    
    player.lifelinesUsed[0] = 1;
    lifelineStack.usedLifeline(0);
    return gameLogic.apply50_50Lifeline(currentQuestion);
}

int GameEngine::useAskFriendLifeline() {
    if (!gameLogic.isLifelineAvailable(1)) return -1;
    
    player.lifelinesUsed[1] = 1;
    lifelineStack.usedLifeline(1);
    return gameLogic.applyAskFriendLifeline(currentQuestion);
}

bool GameEngine::useSkipLifeline() {
    if (!gameLogic.isLifelineAvailable(2)) return false;
    
    player.lifelinesUsed[2] = 1;
    lifelineStack.usedLifeline(2);
    return gameLogic.applySkipLifeline();
}

string GameEngine::useHintLifeline() {
    if (!gameLogic.isLifelineAvailable(3)) return "";
    
    player.lifelinesUsed[3] = 1;
    lifelineStack.usedLifeline(3);
    return gameLogic.applyHintLifeline(currentQuestion);
}

bool GameEngine::isLifelineAvailable(int lifelineType) const {
    return gameLogic.isLifelineAvailable(lifelineType);
}

void GameEngine::endGame() {
    gameActive = false;
    gameTimer.stop();
    showGameStatus = true; // Show status at end
    
    playerProfileManager.updatePlayerStats(
        player.name,
        player.totalWinnings,
        player.currentLevel,
        player.questionsAnswered
    );
    
    auto now = chrono::system_clock::now();
    auto time = chrono::system_clock::to_time_t(now);
    leaderboard.addEntry(player, ctime(&time));
}

void GameEngine::displayStartStatus() {
    cout << "\n╔════════════════════════════════════════════════════╗\n";
    cout << "║              PLAYER PROFILE                        ║\n";
    cout << "║════════════════════════════════════════════════════║\n";
    cout << "║ Name: " << setw(40) << left << player.name << " ║\n";
    cout << "║ Gender: " << setw(38) << left << player.gender << " ║\n";
    cout << "║ Starting Level: 0                              ║\n";
    cout << "║════════════════════════════════════════════════════║\n";
    cout << "║        Category: " << setw(30) << left << questionBank.getCategoryName(currentQuestion.category) << " ║\n";
    cout << "╚════════════════════════════════════════════════════╝\n";
}

void GameEngine::displayEndStatus() {
    cout << "\n╔════════════════════════════════════════════════════╗\n";
    cout << "║              FINAL GAME STATUS                     ║\n";
    cout << "║════════════════════════════════════════════════════║\n";
    cout << "║ Player: " << setw(37) << left << player.name << " ║\n";
    cout << "║ Final Level: " << setw(33) << left << player.currentLevel << " ║\n";
    cout << "║ Total Winnings: Rs. " << setw(25) << left << player.totalWinnings << " ║\n";
    cout << "║ Questions Answered: " << setw(25) << left << player.questionsAnswered << " ║\n";
    cout << "║════════════════════════════════════════════════════║\n";
    quoteManager.displayRandomQuote(); // Display random quote
}

void GameEngine::displayGameStatus() {
}

void GameEngine::displayQuestion() {
    cout << "\n[Level " << player.currentLevel << "] " << ": " 
         << currentQuestion.text << "\n\n";
    for (int i = 0; i < 4; i++) {
        cout << (char)('A' + i) << ") " << currentQuestion.options[i] << "\n";
    }
    cout << "\nLifelines: ";
    cout << "50-50(" << (player.lifelinesUsed[0] ? "X" : "✓") << ") ";
    cout << "AskFriend(" << (player.lifelinesUsed[1] ? "X" : "✓") << ") ";
    cout << "Skip(" << (player.lifelinesUsed[2] ? "X" : "✓") << ") ";
    cout << "Hint(" << (player.lifelinesUsed[3] ? "X" : "✓") << ")\n"; // 
    cout << "Time: ";
    gameTimer.displayTimer();
    cout << "\n";
}

const Player& GameEngine::getPlayer() const { return player; }
const PrizeLadder& GameEngine::getPrizeLadder() const { return prizeLadder; }
const Question& GameEngine::getCurrentQuestion() const { return currentQuestion; }
Leaderboard& GameEngine::getLeaderboard() { return leaderboard; }
int GameEngine::getTotalPoints() const { return totalPointsEarned; }
int GameEngine::getCorrectStreak() const { return correctAnswerStreak; }

PlayerStats GameEngine::getPlayerProfile()  {
    if (playerProfileManager.playerExists(player.name)) {
        return playerProfileManager.getOrCreateProfile(player.name, player.gender);
    }
    return PlayerStats(player.name, player.gender);
}

PlayerProfileManager& GameEngine::getProfileManager() { return playerProfileManager; }
