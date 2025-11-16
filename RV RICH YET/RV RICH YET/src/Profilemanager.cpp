#include "ProfileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

/*
=========================================================
 load() — Reads all player data from file
=========================================================

File format:
    <playerName>
    <highScore> <gamesPlayed> <totalScore>
    QID1,QID2,QID3,...

We use:
✔ getline() for safe line-by-line reading
✔ stringstream for parsing score fields
✔ unordered_set for storing question history
*/
bool ProfileManager::load(const std::string& filename)
{
    profiles.clear();

    ifstream in(filename);
    if (!in.is_open())
        return false;   // No file yet — acceptable

    string line;

    while (getline(in, line))
    {
        if (line.empty()) continue;

        PlayerProfile p;
        p.name = line;      // First line = player name

        // Second line = numeric stats
        if (!getline(in, line)) break;
        stringstream ss(line);
        ss >> p.highScore >> p.gamesPlayed >> p.totalScore;

        // Third line = lifetime question history
        if (!getline(in, line)) break;
        stringstream qs(line);
        string qid;

        // Split by comma manually
        while (getline(qs, qid, ','))
        {
            if (!qid.empty())
                p.questionHistory.insert(qid);
        }

        // Store in hash table
        profiles[p.name] = p;
    }

    in.close();
    return true;
}

/*
=========================================================
 save() — Writes all player data back to file
=========================================================

We output:
- name
- "highScore gamesPlayed totalScore"
- comma-separated used question IDs

Why text file?
✔ Easy to debug
✔ Easy for TA to check
✔ Works on all OS
*/
bool ProfileManager::save(const std::string& filename)
{
    ofstream out(filename, ios::trunc);
    if (!out.is_open())
        return false;

    for (auto& kv : profiles)
    {
        auto& p = kv.second;

        out << p.name << "\n";
        out << p.highScore << " " << p.gamesPlayed << " " << p.totalScore << "\n";

        // Join unordered_set with commas
        bool first = true;
        for (auto& id : p.questionHistory)
        {
            if (!first) out << ",";
            out << id;
            first = false;
        }
        out << "\n";
    }

    out.close();
    return true;
}

/*
=========================================================
 getOrCreate() — O(1) hash table lookup
=========================================================

If profile exists → return it  
If not → create a new one

DSA:
✔ unordered_map gives O(1) insertion & lookup
*/
PlayerProfile& ProfileManager::getOrCreate(const std::string& name)
{
    if (profiles.find(name) == profiles.end())
    {
        PlayerProfile p;
        p.name = name;
        profiles[name] = p;
    }

    return profiles[name];
}

bool ProfileManager::exists(const std::string& name) const
{
    return profiles.find(name) != profiles.end();
}

/*
=========================================================
 updateScore() — Final scoring after each game
=========================================================

Updates:
- gamesPlayed++
- totalScore += score
- highScore = max(...)
*/
void ProfileManager::updateScore(const std::string& name, int score)
{
    PlayerProfile& p = profiles[name];

    p.gamesPlayed += 1;
    p.totalScore += score;

    if (score > p.highScore)
        p.highScore = score;
}

/*
=========================================================
 markQuestionUsed() — Add lifetime history
=========================================================

DSA:
✔ unordered_set — O(1) insert
Prevents repeated questions across multiple sessions.
*/
void ProfileManager::markQuestionUsed(const std::string& name, const std::string& qId)
{
    profiles[name].questionHistory.insert(qId);
}

/*
=========================================================
 allProfiles() — returns full hash map
=========================================================

Used by Leaderboard (merge sort) module.
*/
std::unordered_map<std::string, PlayerProfile> ProfileManager::allProfiles() const
{
    return profiles;
}
