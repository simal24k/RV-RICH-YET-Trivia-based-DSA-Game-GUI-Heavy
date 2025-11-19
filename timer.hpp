#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <iostream>

using namespace std;

class GameTimer {
private:
    chrono::system_clock::time_point startTime;
    int durationSeconds;
    bool isRunning;

public:
    GameTimer(int duration = 30);
    
    void start();
    void stop();
    void setDuration(int seconds);
    int getElapsedSeconds() const;
    int getRemainingSeconds() const;
    bool isTimeUp() const;
    void displayTimer() const;
};

#endif
