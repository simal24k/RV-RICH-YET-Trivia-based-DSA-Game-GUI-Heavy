#include "player_profile.hpp"
#include <vector>
using namespace std;

PlayerProfileManager::PlayerProfileManager() {
    loadProfiles();
}

PlayerStats PlayerProfileManager::getOrCreateProfile(const string& name, const string& gender) {
    if (profiles.find(name) == profiles.end()) {
        profiles[name] = PlayerStats(name, gender);
    }
    return profiles[name];
}

void PlayerProfileManager::updatePlayerStats(const string& name, long long winnings, int level, int questionsAnswered) {
    if (profiles.find(name) != profiles.end()) {
        profiles[name].gamesPlayed++;
        profiles[name].totalWinnings += winnings;
        if (level > profiles[name].maxLevel) {
            profiles[name].maxLevel = level;
        }
        saveProfiles();
    }
}

bool PlayerProfileManager::playerExists(const string& name) const {
    return profiles.find(name) != profiles.end();
}

void PlayerProfileManager::saveProfiles() const {
    ofstream file(filename);
    for (const auto& entry : profiles) {
        file << entry.first << "|" << entry.second.gender << "|"
             << entry.second.gamesPlayed << "|" << entry.second.totalWinnings << "|"
             << entry.second.maxLevel << "\n";
    }
    file.close();
}

void PlayerProfileManager::loadProfiles() {
    ifstream file(filename);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        vector<string> tokens;
        stringstream ss(line);
        string token;

        while (getline(ss, token, '|')) {
            tokens.push_back(token);
        }

        if (tokens.size() >= 5) {
            PlayerStats stats(tokens[0], tokens[1]);
            stats.gamesPlayed = stoi(tokens[2]);
            stats.totalWinnings = stoll(tokens[3]);
            stats.maxLevel = stoi(tokens[4]);
            profiles[tokens[0]] = stats;
        }
    }
    file.close();
}
