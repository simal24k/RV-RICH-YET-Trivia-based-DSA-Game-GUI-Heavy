#include "timer.hpp"

using namespace std;

GameTimer::GameTimer(int duration) : durationSeconds(duration), isRunning(false) {}

void GameTimer::start()
{
    startTime = chrono::system_clock::now();
    isRunning = true;
}

void GameTimer::stop()
{
    isRunning = false;
}

void GameTimer::setDuration(int seconds)
{
    durationSeconds = seconds;
}

// FIX: Added empty update method (chrono doesn't need explicit updates, but controller calls it)
void GameTimer::update()
{
    // No-op for system_clock implementation
}

// FIX: Added isFinished implementation
bool GameTimer::isFinished() const
{
    return getRemainingSeconds() <= 0;
}

int GameTimer::getElapsedSeconds() const
{
    if (!isRunning)
        return 0;
    auto now = chrono::system_clock::now();
    return chrono::duration_cast<chrono::seconds>(now - startTime).count();
}

int GameTimer::getRemainingSeconds() const
{
    int elapsed = getElapsedSeconds();
    return max(0, durationSeconds - elapsed);
}

void GameTimer::displayTimer() const
{
    cout << getRemainingSeconds() << "s";
}