#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>
using namespace std;
class GameTimer {
private:
     chrono::steady_clock::time_point startTime;
    int durationSeconds;
    bool isRunning;

public:
    GameTimer(int seconds = 30) 
        : durationSeconds(seconds), isRunning(false) {}

    void start() {
        startTime =  chrono::steady_clock::now();
        isRunning = true;
    }

    void stop() {
        isRunning = false;
    }

    int getElapsedSeconds() const {
        if (!isRunning) return 0;
        
        auto now =  chrono::steady_clock::now();
        auto elapsed =  chrono::duration_cast< chrono::seconds>(now - startTime);
        return elapsed.count();
    }

    int getRemainingSeconds() const {
        int remaining = durationSeconds - getElapsedSeconds();
        return remaining > 0 ? remaining : 0;
    }

    bool isTimeUp() const {
        return getRemainingSeconds() == 0;
    }

    bool isRunning_() const {
        return isRunning;
    }

    void setDuration(int seconds) {
        durationSeconds = seconds;
    }

    void displayTimer() const {
        int remaining = getRemainingSeconds();
        int minutes = remaining / 60;
        int seconds = remaining % 60;
        
         cout << "Time Remaining: ";
        if (minutes > 0) {
             cout << minutes << "m " << seconds << "s";
        } else {
             cout << seconds << "s";
        }
    }
};

#endif
