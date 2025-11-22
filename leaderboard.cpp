#include "leaderboard.hpp"
#include <iomanip>
#include <vector>


using namespace std;

Leaderboard::Leaderboard() {
    loadFromFile();
}

void Leaderboard::mergeSort(vector<LeaderboardEntry>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void Leaderboard::merge(vector<LeaderboardEntry>& arr, int left, int mid, int right) {
    vector<LeaderboardEntry> temp;
    int i = left, j = mid + 1;

    while (i <= mid && j <= right) {
        if (arr[i] < arr[j]) {
            temp.push_back(arr[i++]);
        } else {
            temp.push_back(arr[j++]);
        }
    }

    while (i <= mid) temp.push_back(arr[i++]);
    while (j <= right) temp.push_back(arr[j++]);

    for (int i = left, k = 0; i <= right; i++, k++) {
        arr[i] = temp[k];
    }
}

void Leaderboard::addEntry(const Player& player, const string& timestamp) {
    entries.push_back({
        player.name, 
        player.totalWinnings, 
        player.currentLevel, 
        timestamp,
        player.questionsAnswered
    });

    if (entries.size() > MAX_ENTRIES) {
        sortEntries();
        entries.resize(MAX_ENTRIES);
    }

    sortEntries();
    saveToFile();
}

void Leaderboard::sortEntries() {
    if (entries.size() <= 1) return;
    mergeSort(entries, 0, entries.size() - 1);
}

void Leaderboard::saveToFile() const {
    ofstream file(filename);
    for (const auto& entry : entries) {
        file << entry.playerName << "|" << entry.winnings << "|" 
             << entry.level << "|"
             << entry.gamesPlayed << "\n";
    }
    file.close();
}

void Leaderboard::loadFromFile() {
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
            entries.push_back({
                tokens[0], 
                stoll(tokens[1]), 
                stoi(tokens[2]), 
                tokens[3],
                stoi(tokens[4])
            });
        }
    }
    file.close();
    sortEntries();
}

const vector<LeaderboardEntry>& Leaderboard::getTopEntries(int count) const {
    static vector<LeaderboardEntry> top;
    top.clear();
    for (int i = 0; i < min(count, (int)entries.size()); i++) {
        top.push_back(entries[i]);
    }
    return top;
}

vector<LeaderboardEntry> Leaderboard::getPlayerHistory(const string& playerName) const {
    vector<LeaderboardEntry> history;
    for (const auto& entry : entries) {
        if (entry.playerName == playerName) {
            history.push_back(entry);
        }
    }
    return history;
}

void Leaderboard::display() const {
    cout << "\n╔════════════════════════════════════════════════════╗\n";
    cout << "║          TOP 5 PLAYERS - LEADERBOARD             ║\n"; // 
    cout << "║════════════════════════════════════════════════════║\n";
    cout << "║Rank │ Name              │ Winnings      │ Level  ║\n";
    cout << "║─────┼───────────────────┼───────────────┼────────║\n";
    
    int rank = 1;
    for (const auto& entry : getTopEntries(TOP_DISPLAY)) { // Show only TOP_DISPLAY
        cout << "║" << setw(4) << rank << " │ " 
             << setw(17) << left << entry.playerName << "│ Rs."
             << setw(10) << right << entry.winnings << "   │ "
             << setw(5) << entry.level << "  ║\n";
        rank++;
    }

    cout << "╚════════════════════════════════════════════════════╝\n";
}

void Leaderboard::displayPlayerHistory(const string& playerName) const {
    auto history = getPlayerHistory(playerName);
    
    if (history.empty()) {
        cout << "No history found for player: " << playerName << "\n";
        return;
    }

    cout << "\n╔════════════════════════════════════════════════════╗\n";
    cout << "║        GAME HISTORY - " << setw(24) << left << playerName << "║\n";
    cout << "║════════════════════════════════════════════════════║\n";
    cout << "║ Game │ Winnings      │ Level │   ║\n";
    cout << "║──────┼───────────────┼───────┼───║\n";

    int gameNum = 1;
    for (const auto& entry : history) {
        cout << "║ " << setw(4) << gameNum << " │ Rs." << setw(10) 
             << entry.winnings << "   │ " << setw(5) << entry.level 
             << " │ " << setw(16) << left << entry.timestamp.substr(0, 16) 
             << "║\n";
        gameNum++;
    }

    cout << "╚════════════════════════════════════════════════════╝\n";
}

int Leaderboard::getTotalGames() const { return entries.size(); }

long long Leaderboard::getTotalPrizePool() const {
    long long total = 0;
    for (const auto& entry : entries) {
        total += entry.winnings;
    }
    return total;
}

int Leaderboard::getAverageLevel() const {
    if (entries.empty()) return 0;
    int sum = 0;
    for (const auto& entry : entries) {
        sum += entry.level;
    }
    return sum / entries.size();
}
