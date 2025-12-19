#ifndef LEADERBOARD_HPP
#define LEADERBOARD_HPP

#include "data_structures.hpp"
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;

struct LeaderboardEntry
{
    string playerName;
    long long winnings;
    int level;
    int gamesPlayed;

    bool operator<(const LeaderboardEntry &other) const
    {
        if (winnings != other.winnings)
            return winnings > other.winnings;
        return level > other.level;
    }

    bool operator>(const LeaderboardEntry &other) const
    {
        return other < *this;
    }
};

class Leaderboard
{
private:
    vector<LeaderboardEntry> entries;
 
    const string filename = "docs/leaderboard.txt";

    static constexpr int MAX_ENTRIES = 100;
    static constexpr int TOP_DISPLAY = 5;

    void mergeSort(vector<LeaderboardEntry> &arr, int left, int right);
    void merge(vector<LeaderboardEntry> &arr, int left, int mid, int right);



public:
    Leaderboard();

    void addEntry(const Player &player, const string &timestamp);
    void sortEntries();
    void saveToFile() const;
    void loadFromFile();

    const vector<LeaderboardEntry> &getTopEntries(int count = 5) const;
    vector<LeaderboardEntry> getPlayerHistory(const string &playerName) const;

    int getTotalGames() const;
    long long getTotalPrizePool() const;
    int getAverageLevel() const;
    
};

#endif