#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>

/*
---------------------------------------------------------
 PlayerProfile
---------------------------------------------------------

This struct stores ALL persistent data associated with a player.

DSA choices:
✔ unordered_set<string>  (hash set)
    - Used for storing lifetime question history.
    - O(1) average insertion & lookup.
    - Ensures we never repeat questions for a player over multiple games.

Fields:
- highScore:     Best prize achieved.
- gamesPlayed:   How many times they've played.
- totalScore:    Sum of all winnings.
- name:          Player identifier.
*/
struct PlayerProfile
{
    std::string name;
    int highScore = 0;
    int gamesPlayed = 0;
    int totalScore = 0;

    // Hash set: fast O(1) lookup for repeated question history
    std::unordered_set<std::string> questionHistory;
};


/*
---------------------------------------------------------
 ProfileManager Class
---------------------------------------------------------

Purpose:
- Manages ALL player profiles at runtime.
- Loads from / saves to a text file.
- Provides methods to update stats and history.
- Provides data for leaderboard sorting.

DSA backbone:
✔ unordered_map<string, PlayerProfile>
    - Hash table mapping playerName -> PlayerProfile.
    - O(1) insertion, lookup, modification.
*/
class ProfileManager
{
public:

    // Load and save profiles to a simple text file
    bool load(const std::string& filename);
    bool save(const std::string& filename);

    // Retrieve or auto-create a player
    PlayerProfile& getOrCreate(const std::string& name);

    // Check if a profile exists
    bool exists(const std::string& name) const;

    // Update highscore, games played, etc.
    void updateScore(const std::string& name, int score);

    // Track questions this player has already seen
    void markQuestionUsed(const std::string& name, const std::string& qId);

    // Return all profiles for leaderboard sorting
    std::unordered_map<std::string, PlayerProfile> allProfiles() const;

private:
    // Hash table storing ALL players
    std::unordered_map<std::string, PlayerProfile> profiles;
};