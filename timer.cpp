#include "timer.hpp"

using namespace std;

GameTimer::GameTimer(int duration) : durationSeconds(duration), isRunning(false) {}

void GameTimer::start() {
    startTime = chrono::system_clock::now();
    isRunning = true;
}

void GameTimer::stop() {
    isRunning = false;
}

void GameTimer::setDuration(int seconds) {
    durationSeconds = seconds;
}

int GameTimer::getElapsedSeconds() const {
    if (!isRunning) return 0;
    auto now = chrono::system_clock::now();
    return chrono::duration_cast<chrono::seconds>(now - startTime).count();
}

int GameTimer::getRemainingSeconds() const {
    int elapsed = getElapsedSeconds();
    return max(0, durationSeconds - elapsed);
}

bool GameTimer::isTimeUp() const {
    return getRemainingSeconds() <= 0;
}

void GameTimer::displayTimer() const {
    cout << getRemainingSeconds() << "s";
}
