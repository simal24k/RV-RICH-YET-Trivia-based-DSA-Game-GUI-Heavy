#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <iostream>

using namespace std;

class GameTimer
{
private:
    chrono::system_clock::time_point startTime;
    int durationSeconds;
    bool isRunning;

public:
    GameTimer(int duration = 30);

    void start();
    void stop();
    void setDuration(int seconds);

    // FIX: Added missing methods to match GameController usage
    void update();
    bool isFinished() const;

    int getElapsedSeconds() const;
    int getRemainingSeconds() const;
    void displayTimer() const;
};

#endif