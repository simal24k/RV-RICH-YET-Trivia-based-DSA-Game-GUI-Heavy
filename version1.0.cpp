// dsa_trivia_full.cpp
// Compile (g++ / clang):  g++ -std=c++17 dsa_trivia_full.cpp -o dsa_trivia
// Compile (MSVC):         cl /EHsc /std:c++17 dsa_trivia_full.cpp
//
// Expects question file in one of:
//   data/questions.txt    (recommended)
// or
//   questions.txt
//
// Question format (pipe-separated):
// ID|Category|Difficulty|Question|OptionA|OptionB|OptionC|OptionD|CorrectIndex|Hint
//
// Example:
// 1|Sorting|2|What is an ADT?|A model of data+operations|A code library|A compiler feature|A sorting rule|0|Concept layer, not implementation.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <stack>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

/* =====================================================
   Data models
   ===================================================== */

// Question struct matches new format:
// ID | Category | Difficulty | Question | A | B | C | D | CorrectIndex | Hint
struct Question {
    string id;
    string category;
    int difficulty;         // 1..5
    string text;
    array<string,4> opts;
    int correctIndex;       // 0..3
    string hint;
};

// Player profile (stored in-memory + persisted)
struct PlayerProfile {
    string name;
    int highScore = 0;
    int gamesPlayed = 0;
    int totalScore = 0;
};

/* =====================================================
   Utility: split by pipe
   ===================================================== */
vector<string> splitPipe(const string &line) {
    vector<string> parts;
    string cur;
    for (char c : line) {
        if (c == '|') {
            parts.push_back(cur);
            cur.clear();
        } else cur.push_back(c);
    }
    parts.push_back(cur);
    return parts;
}

/* =====================================================
   Load questions
   ===================================================== */
vector<Question> loadQuestionsFromFile(const string &filename) {
    vector<Question> out;
    ifstream in(filename);
    if (!in.is_open()) {
        // silent fail; caller may try alternate path
        return out;
    }
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        auto parts = splitPipe(line);
        // Expect 10 fields
        if (parts.size() < 10) continue;
        Question q;
        q.id = parts[0];
        q.category = parts[1];
        try {
            q.difficulty = stoi(parts[2]);
        } catch(...) { q.difficulty = 1; }
        q.text = parts[3];
        q.opts[0] = parts[4];
        q.opts[1] = parts[5];
        q.opts[2] = parts[6];
        q.opts[3] = parts[7];
        try {
            q.correctIndex = stoi(parts[8]);
        } catch(...) { q.correctIndex = 0; }
        q.hint = parts[9];
        out.push_back(q);
    }
    in.close();
    return out;
}

/* =====================================================
   Prize ladder (singly linked list)
   DSA: Linked List models the linear ladder.
   ===================================================== */
struct LadderNode {
    int amount;
    bool safety;
    LadderNode* next;
    LadderNode(int a=0, bool s=false) : amount(a), safety(s), next(nullptr) {}
};

class PrizeLadder {
    LadderNode* head;
public:
    PrizeLadder(): head(nullptr) {}
    ~PrizeLadder() {
        while (head) {
            LadderNode* t = head;
            head = head->next;
            delete t;
        }
    }
    void buildDefault() {
        vector<int> amounts = {100,200,300,500,1000,2000,4000,8000,16000,32000,64000,125000,250000,500000,1000000};
        LadderNode* tail = nullptr;
        for (size_t i=0;i<amounts.size();++i) {
            bool safe = (i==4 || i==9); // 5th and 10th as safety
            LadderNode* node = new LadderNode(amounts[i], safe);
            if (!head) head = tail = node;
            else { tail->next = node; tail = node; }
        }
    }
    int getAmountAtIndex(int idx0) const {
        LadderNode* cur = head;
        int i = 0;
        while (cur) {
            if (i == idx0) return cur->amount;
            cur = cur->next; ++i;
        }
        return 0;
    }
    int lastSafetyIndexBefore(int idx1_based) const {
        LadderNode* cur = head;
        int i = 1;
        int lastSafe = 0;
        while (cur && i <= idx1_based) {
            if (cur->safety) lastSafe = i;
            cur = cur->next; ++i;
        }
        return lastSafe;
    }
    int size() const {
        int cnt=0;
        for (LadderNode* cur=head; cur; cur=cur->next) ++cnt;
        return cnt;
    }
    void printProgress(int currentIndex) const {
        LadderNode* cur = head;
        int i=1;
        while (cur) {
            if (i <= currentIndex) cout << "[" << cur->amount << "]";
            else cout << " " << cur->amount << " ";
            if (cur->safety) cout << "(S) ";
            else cout << " ";
            cur = cur->next; ++i;
        }
        cout << "\n";
    }
};

/* =====================================================
   Lifelines: simple stack wrapper (consumable lifelines)
   ===================================================== */
enum LifelineType { L50_50, ASK_FRIEND, SKIP };

string lifelineName(LifelineType L) {
    switch(L) {
        case L50_50: return "50-50";
        case ASK_FRIEND: return "Ask a Friend";
        case SKIP: return "Skip";
    }
    return "Unknown";
}

template<typename T>
class SimpleStack {
    vector<T> data;
public:
    SimpleStack() = default;
    void push(const T &v) { data.push_back(v); }
    T pop() {
        if (data.empty()) throw out_of_range("Stack empty");
        T t = data.back(); data.pop_back(); return t;
    }
    T top() const {
        if (data.empty()) throw out_of_range("Stack empty");
        return data.back();
    }
    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }
};

/* =====================================================
   Difficulty scaling: Max-Heap (priority_queue)
   DSA: priority_queue is used to choose higher-difficulty questions first.
   ===================================================== */
struct PQItem {
    int difficulty;
    int seq;
    Question q;
    bool operator<(PQItem const& other) const {
        if (difficulty != other.difficulty) return difficulty < other.difficulty;
        return seq > other.seq;
    }
};
priority_queue<PQItem> questionHeap;

/* Build heap from vector */
void buildQuestionHeap(const vector<Question>& qs) {
    while (!questionHeap.empty()) questionHeap.pop();
    int seq=0;
    for (const auto &q : qs) {
        questionHeap.push(PQItem{q.difficulty, seq++, q});
    }
}

/* =====================================================
   Profiles persistence and leaderboard (merge sort)
   ===================================================== */
const string DEFAULT_DATA_DIR = "data/";
const string QUESTIONS_FILE1 = DEFAULT_DATA_DIR + "questions.txt";
const string PROFILES_FILE = DEFAULT_DATA_DIR + "profiles.txt";

unordered_map<string, PlayerProfile> profiles;

void loadProfiles(const string &filename) {
    profiles.clear();
    ifstream in(filename);
    if (!in.is_open()) return;
    string name;
    while (getline(in, name)) {
        if (name.empty()) continue;
        string line;
        if (!getline(in, line)) break;
        stringstream ss(line);
        PlayerProfile p; p.name = name;
        ss >> p.highScore >> p.gamesPlayed >> p.totalScore;
        profiles[p.name] = p;
    }
    in.close();
}

void saveProfiles(const string &filename) {
    ofstream out(filename, ios::trunc);
    if (!out.is_open()) {
        cerr << "Failed to save profiles to " << filename << "\n";
        return;
    }
    for (auto &kv : profiles) {
        auto &p = kv.second;
        out << p.name << "\n";
        out << p.highScore << " " << p.gamesPlayed << " " << p.totalScore << "\n";
    }
    out.close();
}

// Merge sort for leaderboard (DSA: explicit merge sort)
vector<PlayerProfile> mergeSortedProfiles(const vector<PlayerProfile>& a, const vector<PlayerProfile>& b) {
    vector<PlayerProfile> res;
    size_t i=0,j=0;
    while (i<a.size() && j<b.size()) {
        if (a[i].highScore > b[j].highScore) res.push_back(a[i++]);
        else if (a[i].highScore < b[j].highScore) res.push_back(b[j++]);
        else {
            if (a[i].totalScore >= b[j].totalScore) res.push_back(a[i++]);
            else res.push_back(b[j++]);
        }
    }
    while (i<a.size()) res.push_back(a[i++]);
    while (j<b.size()) res.push_back(b[j++]);
    return res;
}

vector<PlayerProfile> mergeSortProfiles(const vector<PlayerProfile>& arr) {
    if (arr.size() <= 1) return arr;
    size_t mid = arr.size()/2;
    vector<PlayerProfile> left(arr.begin(), arr.begin()+mid);
    vector<PlayerProfile> right(arr.begin()+mid, arr.end());
    left = mergeSortProfiles(left);
    right = mergeSortProfiles(right);
    return mergeSortedProfiles(left, right);
}

/* =====================================================
   UI / Asking questions
   ===================================================== */
void showQuestionHeader(const Question &q) {
    cout << "\n[" << q.category << "] (Diff " << q.difficulty << ") " << q.text << "\n";
    for (int i=0;i<4;++i) {
        cout << char('A'+i) << ") " << q.opts[i] << "\n";
    }
}

int askForAnswer(const Question &q, SimpleStack<LifelineType> &lifelines, bool &used50) {
    while (true) {
        cout << "Enter option (A-D), H for hint, L for lifeline: ";
        string in; getline(cin, in);
        if (in.empty()) continue;
        char c = toupper(in[0]);
        if (c >= 'A' && c <= 'D') return c - 'A';
        else if (c == 'H') {
            cout << "Hint: " << q.hint << "\n";
        } else if (c == 'L') {
            if (lifelines.empty()) { cout << "No lifelines left.\n"; continue; }
            cout << "Use lifeline: " << lifelineName(lifelines.top()) << " ? (Y/N): ";
            string y; getline(cin,y);
            if (!y.empty() && toupper(y[0])=='Y') {
                LifelineType L = lifelines.pop();
                if (L == L50_50) {
                    if (used50) cout << "50-50 already used for this question.\n";
                    else {
                        used50 = true;
                        cout << "50-50 applied. Remaining options:\n";
                        cout << char('A' + q.correctIndex) << ") " << q.opts[q.correctIndex] << "\n";
                        vector<int> wrong;
                        for (int i=0;i<4;++i) if (i!=q.correctIndex) wrong.push_back(i);
                        srand((unsigned)time(nullptr));
                        int pick = wrong[rand() % wrong.size()];
                        cout << char('A' + pick) << ") " << q.opts[pick] << "\n";
                    }
                } else if (L == ASK_FRIEND) {
                    srand((unsigned)time(nullptr));
                    int prob = rand()%100;
                    if (prob < 70) cout << "Friend suggests: " << char('A' + q.correctIndex) << "\n";
                    else {
                        vector<int> wrong;
                        for (int i=0;i<4;++i) if (i!=q.correctIndex) wrong.push_back(i);
                        cout << "Friend suggests: " << char('A' + wrong[rand()%wrong.size()]) << "\n";
                    }
                } else if (L == SKIP) {
                    cout << "Skip used: question considered correct.\n";
                    return -2;
                }
            }
        } else if (c == 'S') {
            cout << "DEBUG: correct is " << char('A'+q.correctIndex) << "\n";
        } else {
            cout << "Invalid input.\n";
        }
    }
}

/* =====================================================
   Game loop
   ===================================================== */
void playGame(const string &playerName, PrizeLadder &ladder) {
    // ensure profile
    if (profiles.find(playerName) == profiles.end()) {
        PlayerProfile p; p.name = playerName; profiles[playerName] = p;
    }
    PlayerProfile &player = profiles[playerName];

    // lifelines stack
    SimpleStack<LifelineType> lifelines;
    lifelines.push(SKIP);
    lifelines.push(ASK_FRIEND);
    lifelines.push(L50_50);

    int currentIndex = 0; // how many correct so far
    int ladderSize = ladder.size();
    int currentAmount = 0;

    // copy heap so original remains intact for later plays
    priority_queue<PQItem> heapCopy = questionHeap;

    cout << "\nStarting game for " << playerName << "!\n";
    cout << "Prize ladder:\n";
    ladder.printProgress(currentIndex);

    while (currentIndex < ladderSize) {
        if (heapCopy.empty()) {
            cout << "No questions left.\n";
            break;
        }
        PQItem item = heapCopy.top(); heapCopy.pop();
        Question q = item.q;

        // Display header and question
        cout << "\nFor amount: " << ladder.getAmountAtIndex(currentIndex) << "\n";
        showQuestionHeader(q);

        bool used50 = false;
        int ans = askForAnswer(q, lifelines, used50);

        if (ans == -2) {
            // skip lifeline: treat as correct
            currentIndex++;
            currentAmount = ladder.getAmountAtIndex(currentIndex);
            cout << "Skipped. You advance to " << currentAmount << "\n";
            ladder.printProgress(currentIndex);
            continue;
        }

        if (ans == q.correctIndex) {
            cout << "Correct!\n";
            currentIndex++;
            currentAmount = ladder.getAmountAtIndex(currentIndex);
            cout << "You've won: " << currentAmount << "\n";
            ladder.printProgress(currentIndex);
            if (currentIndex == ladderSize) {
                cout << "You've completed the ladder! Congratulations!\n";
                break;
            }
            cout << "Continue? (Y to continue, any other to walk away): ";
            string in; getline(cin,in);
            if (in.empty() || toupper(in[0])!='Y') {
                cout << "You walk away with " << currentAmount << "\n";
                break;
            }
        } else {
            cout << "Incorrect. Correct answer was: " << char('A' + q.correctIndex) << ") " << q.opts[q.correctIndex] << "\n";
            int lastSafe = ladder.lastSafetyIndexBefore(currentIndex+1);
            int safeAmount = (lastSafe==0)?0:ladder.getAmountAtIndex(lastSafe-1); // lastSafe is 1-based index
            cout << "You fall back to safety amount: " << safeAmount << "\n";
            currentAmount = safeAmount;
            break;
        }
    }

    // update profile
    player.gamesPlayed += 1;
    player.totalScore += currentAmount;
    if (currentAmount > player.highScore) player.highScore = currentAmount;
    cout << "Game over. Score: " << currentAmount << "\n";
}

/* =====================================================
   Leaderboard display
   ===================================================== */
void showLeaderboard() {
    vector<PlayerProfile> arr;
    for (auto &kv : profiles) arr.push_back(kv.second);
    auto sorted = mergeSortProfiles(arr);
    cout << "\n=== Leaderboard (by high score) ===\n";
    int rank=1;
    for (auto &p : sorted) {
        cout << rank++ << ". " << p.name << " | High: " << p.highScore << " | Total: " << p.totalScore << " | Games: " << p.gamesPlayed << "\n";
        if (rank>20) break;
    }
    cout << "===================================\n";
}

/* =====================================================
   Main
   ===================================================== */
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Try default paths
    vector<Question> questions;
    questions = loadQuestionsFromFile(QUESTIONS_FILE1);
    if (questions.empty()) {
        // fallback to current dir
        questions = loadQuestionsFromFile("questions.txt");
    }
    if (questions.empty()) {
        cerr << "No questions loaded. Place questions file at '" << QUESTIONS_FILE1 << "' or 'questions.txt'.\n";
        return 1;
    }

    // Build heap (priority_queue) for difficulty scaling (DSA: Max-Heap)
    buildQuestionHeap(questions);

    // Load profiles
    loadProfiles(PROFILES_FILE);

    // Build prize ladder
    PrizeLadder ladder;
    ladder.buildDefault();

    cout << "Welcome to DSA Trivia!\n";
    cout << "Enter your player name: ";
    string playerName; getline(cin, playerName);
    if (playerName.empty()) playerName = "Player";

    while (true) {
        cout << "\nMenu:\n1) Play Game\n2) Leaderboard\n3) View Profile\n4) Save Profiles\n5) Exit\nChoose: ";
        string choice; getline(cin, choice);
        if (choice == "1") {
            playGame(playerName, ladder);
        } else if (choice == "2") {
            showLeaderboard();
        } else if (choice == "3") {
            auto it = profiles.find(playerName);
            if (it == profiles.end()) cout << "No profile found.\n";
            else {
                auto &p = it->second;
                cout << "Profile: " << p.name << " | High: " << p.highScore << " | Total: " << p.totalScore << " | Games: " << p.gamesPlayed << "\n";
            }
        } else if (choice == "4") {
            saveProfiles(PROFILES_FILE);
            cout << "Profiles saved.\n";
        } else if (choice == "5") {
            saveProfiles(PROFILES_FILE);
            cout << "Bye.\n";
            break;
        } else {
            cout << "Invalid.\n";
        }
    }

    return 0;
}
