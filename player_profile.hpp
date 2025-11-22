#ifndef PLAYER_PROFILE_HPP
#define PLAYER_PROFILE_HPP

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

struct PlayerStats {
    string name;
    string gender;
    int gamesPlayed;
    long long totalWinnings;
    int maxLevel;
    double winRate;

    PlayerStats(const string& n = "", const string& g = "")
        : name(n), gender(g), gamesPlayed(0), totalWinnings(0), maxLevel(0), winRate(0.0) {}
};

class PlayerProfileManager {
private:
    unordered_map<string, PlayerStats> profiles;
    const string filename = "player_profiles.txt";

public:
    PlayerProfileManager();
    
    PlayerStats getOrCreateProfile(const string& name, const string& gender);
    void updatePlayerStats(const string& name, long long winnings, int level, int questionsAnswered);
    bool playerExists(const string& name) const;
    void saveProfiles() const;
    void loadProfiles();
};

#endif
