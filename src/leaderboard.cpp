#include "leaderboard.hpp"

using namespace std;

Leaderboard::Leaderboard()
{
    loadFromFile();
}

void Leaderboard::mergeSort(vector<LeaderboardEntry> &arr, int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void Leaderboard::merge(vector<LeaderboardEntry> &arr, int left, int mid, int right)
{
    vector<LeaderboardEntry> temp;
    int i = left, j = mid + 1;

    while (i <= mid && j <= right)
    {
        if (arr[i] < arr[j])
        {
            temp.push_back(arr[i++]);
        }
        else
        {
            temp.push_back(arr[j++]);
        }
    }

    while (i <= mid)
        temp.push_back(arr[i++]);
    while (j <= right)
        temp.push_back(arr[j++]);

    for (int i = left, k = 0; i <= right; i++, k++)
    {
        arr[i] = temp[k];
    }
}

void Leaderboard::addEntry(const Player &player, const string &timestamp)
{
    // We ignore the timestamp argument now that it's removed from LeaderboardEntry
    entries.push_back({player.name,
                       player.totalWinnings,
                       player.currentLevel,
                       player.questionsAnswered});

    if (entries.size() > MAX_ENTRIES)
    {
        sortEntries();
        entries.resize(MAX_ENTRIES);
    }

    sortEntries();
    saveToFile();
}

void Leaderboard::sortEntries()
{
    if (entries.size() <= 1)
        return;
    mergeSort(entries, 0, entries.size() - 1);
}

void Leaderboard::saveToFile() const
{
    ofstream file(filename);
    for (const auto &entry : entries)
    {
        // Saving 4 fields: playerName, winnings, level, gamesPlayed
        file << entry.playerName << "|" << entry.winnings << "|"
             << entry.level << "|"
             << entry.gamesPlayed << "\n";
    }
    file.close();
}

void Leaderboard::loadFromFile()
{
    ifstream file(filename);
    if (!file.is_open())
        return;

    string line;
    while (getline(file, line))
    {
        if (line.empty())
            continue;

        vector<string> tokens;
        stringstream ss(line);
        string token;

        while (getline(ss, token, '|'))
        {
            tokens.push_back(token);
        }

        // Checking for exactly 4 tokens now
        if (tokens.size() == 4)
        {
            entries.push_back({
                tokens[0],
                stoll(tokens[1]),
                stoi(tokens[2]),
                stoi(tokens[3]) 
            });
        }
    }
    file.close();
    sortEntries();
}

const vector<LeaderboardEntry> &Leaderboard::getTopEntries(int count) const
{
    static vector<LeaderboardEntry> top;
    top.clear();
    for (int i = 0; i < min(count, (int)entries.size()); i++)
    {
        top.push_back(entries[i]);
    }
    return top;
}

vector<LeaderboardEntry> Leaderboard::getPlayerHistory(const string &playerName) const
{
    vector<LeaderboardEntry> history;
    for (const auto &entry : entries)
    {
        if (entry.playerName == playerName)
        {
            history.push_back(entry);
        }
    }
    return history;
}

int Leaderboard::getTotalGames() const { return entries.size(); }

long long Leaderboard::getTotalPrizePool() const
{
    long long total = 0;
    for (const auto &entry : entries)
    {
        total += entry.winnings;
    }
    return total;
}

int Leaderboard::getAverageLevel() const
{
    if (entries.empty())
        return 0;
    int sum = 0;
    for (const auto &entry : entries)
    {
        sum += entry.level;
    }
    return sum / entries.size();
}
