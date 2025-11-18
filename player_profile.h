#ifndef PLAYER_PROFILE_H
#define PLAYER_PROFILE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

struct PlayerStats {
     string playerName;
     string gender;
    int gamesPlayed;
    long long totalWinnings;
    int maxLevel;
    int totalCorrectAnswers;
    int totalQuestionsAnswered;
     string lastPlayedDate;
    float winRate;

    PlayerStats() 
        : gamesPlayed(0), totalWinnings(0), maxLevel(0), 
          totalCorrectAnswers(0), totalQuestionsAnswered(0), winRate(0.0f) {}

    PlayerStats(const  string& name, const  string& gen)
        : playerName(name), gender(gen), gamesPlayed(0), totalWinnings(0), 
          maxLevel(0), totalCorrectAnswers(0), totalQuestionsAnswered(0), 
          winRate(0.0f) {}

    void updateWinRate() {
        if (totalQuestionsAnswered > 0) {
            winRate = (float)totalCorrectAnswers / totalQuestionsAnswered * 100.0f;
        }
    }

    void recordGameResult(long long winnings, int level, int questionsAnswered) {
        gamesPlayed++;
        totalWinnings += winnings;
        if (level > maxLevel) maxLevel = level;
        totalQuestionsAnswered += questionsAnswered;
        totalCorrectAnswers += questionsAnswered; // Simplified, actual would need per-question tracking
        updateWinRate();
    }

    void displayStats() const {
         cout << "\n╔════════════════════════════════════════════════════  \n";
         cout << "               PLAYER PROFILE                           \n";
         cout << "                                                        \n";
         cout << "   Name: " <<  setw(40) << playerName << "          \n";
         cout << "   Gender: " <<  setw(38) << gender << "            \n";
         cout << "   Games Played: " <<  setw(33) << gamesPlayed << " \n";
         cout << "   Total Winnings: Rs. " <<  setw(26) << totalWinnings << "   \n";
         cout << "   Max Level Reached: " <<  setw(27) << maxLevel << "   \n";
         cout << "   Win Rate: " <<  fixed <<  setprecision(1) <<  setw(36) << winRate << "%   \n";
         cout << "                                                      \n";
         cout << "╚════════════════════════════════════════════════════╝\n";
    }
};

class PlayerProfileManager {
private:
    mutable  unordered_map< string, PlayerStats> profiles;
    const  string profilesFile = "player_profiles.txt";

     string sanitizeName(const  string& name) const {
         string result;
        for (char c : name) {
            if (c != '|' && c != '\n') {
                result += c;
            }
        }
        return result;
    }

public:
    PlayerProfileManager() {
        loadProfiles();
    }

    ~PlayerProfileManager() {
        saveProfiles();
    }

    bool playerExists(const  string& playerName) const {
        return profiles.find(playerName) != profiles.end();
    }

    PlayerStats& getOrCreateProfile(const  string& playerName, const  string& gender) {
        if (!playerExists(playerName)) {
            profiles[playerName] = PlayerStats(playerName, gender);
        }
        return profiles[playerName];
    }

    const PlayerStats& getOrCreateProfile(const  string& playerName, const  string& gender) const {
        if (!playerExists(playerName)) {
            profiles[playerName] = PlayerStats(playerName, gender);
        }
        return profiles.at(playerName);
    }

    void updatePlayerStats(const  string& playerName, long long winnings, 
                          int level, int questionsAnswered) {
        if (playerExists(playerName)) {
            profiles[playerName].recordGameResult(winnings, level, questionsAnswered);
        }
    }

    void displayPlayerProfile(const  string& playerName) const {
        if (playerExists(playerName)) {
            profiles.at(playerName).displayStats();
        } else {
             cout << "Player not found in profiles.\n";
        }
    }

    void displayAllProfiles() const {
         cout << "\n╔════════════════════════════════════════════════════  \n";
         cout << "             ALL PLAYER PROFILES                      \n";
         cout << "                                                      \n";

        int rank = 1;
        for (const auto& pair : profiles) {
            const auto& stats = pair.second;
            printf("  %2d. %-20s | Rs.%-12lld | Lvl %2d       \n",
                   rank, stats.playerName.c_str(), stats.totalWinnings, stats.maxLevel);
            rank++;
        }

         cout << "                                                      \n";
         cout << "╚════════════════════════════════════════════════════╝\n";
    }

    void saveProfiles() const {
         ofstream file(profilesFile);
        for (const auto& pair : profiles) {
            const auto& stats = pair.second;
            file << stats.playerName << "|" << stats.gender << "|" 
                 << stats.gamesPlayed << "|" << stats.totalWinnings << "|"
                 << stats.maxLevel << "|" << stats.totalCorrectAnswers << "|"
                 << stats.totalQuestionsAnswered << "|" << stats.winRate << "\n";
        }
        file.close();
         cout << "[INFO] Player profiles saved to " << profilesFile << "\n";
    }

    void loadProfiles() {
         ifstream file(profilesFile);
        if (!file.is_open()) return;

         string line;
        while ( getline(file, line)) {
            if (line.empty()) continue;

             vector< string> tokens;
             stringstream ss(line);
             string token;

            while ( getline(ss, token, '|')) {
                tokens.push_back(token);
            }

            if (tokens.size() >= 8) {
                PlayerStats stats(tokens[0], tokens[1]);
                stats.gamesPlayed =  stoi(tokens[2]);
                stats.totalWinnings =  stoll(tokens[3]);
                stats.maxLevel =  stoi(tokens[4]);
                stats.totalCorrectAnswers =  stoi(tokens[5]);
                stats.totalQuestionsAnswered =  stoi(tokens[6]);
                stats.winRate =  stof(tokens[7]);

                profiles[stats.playerName] = stats;
            }
        }
        file.close();
    }

    int getTotalPlayers() const { return profiles.size(); }

    const  unordered_map< string, PlayerStats>& getAllProfiles() const {
        return profiles;
    }
};

#endif
