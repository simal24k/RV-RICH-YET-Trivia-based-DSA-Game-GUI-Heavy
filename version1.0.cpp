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


class Node{
    //prize ladder
    public:
    Node * next;
    int amount;
    bool safety;
    
    Node(int a, bool s){
        next = NULL;
        amount = a;
        safety = s;
    }
};

class PrizeLadder{
    Node * head;
    PrizeLadder(){
        head = NULL;
    }
    ~PrizeLadder(){
    while(head!=NULL){
        Node * temp = head;
        head= head->next;
        delete temp;
    }}
    void prizelist(){
        vector<int> amounts = {100,200,300,500,1000,2000,4000,8000,16000,32000,64000,125000,250000,500000,1000000};
        Node* tail = nullptr;
        for (int i=0;i<amounts.size();i++) {
            bool safe = (i==4 || i==9); // zero-based: 4=5th, 9=10th
            Node* node = new Node(amounts[i], safe);
            if (!head) head = tail = node;
            else { tail->next = node; tail = node; }
        }
    }
    int getAmountAt(int idx) {
        Node* cur = head;
        int i=0;
        while (cur) {
            if (i==idx) return cur->amount;
            cur = cur->next; 
            i++;
        }
        return 0;
    }
    //finding last safety point via traversal in LL: 0,5,10
    int lastSafetyIndexBefore(int idx) {
        Node* cur = head;
        int i=1;
        int lastSafe = 0;
        while (cur && i<=idx) {
            if (cur->safety) lastSafe = i;
            cur = cur->next; ++i;
        }
        return lastSafe;
    }
    int size() {
        int count = 0;
        for (Node * cur = head; cur; cur=cur->next) count++;
        return count;
    }

    //stack for lifelines
enum LifelineType {
    L50_50,  // 50-50 lifeline
    ASK_FRIEND,  // Ask a friend lifeline
    SKIP  // Skip question lifeline
};

// Custom stack class
template<typename T>
class myStack {
private:
    vector<T> stack;
    int max_size;

public:
    // Constructor to initialize the stack with a max size
    myStack(int size) : max_size(size) {}

    // Push an element onto the stack
    void push(const T &value) {
        if (stack.size() < max_size) {
            stack.push_back(value);
        }
    }

    // Pop an element from the stack and return it
    T pop() {
        if (!stack.empty()) {
            T top_value = stack.back();
            stack.pop_back();
            return top_value;
        }
        throw out_of_range("Stack is empty");
    }

    // Peek at the top element without removing it
    T peek() const {
        if (!stack.empty()) {
            return stack.back();
        }
        throw out_of_range("Stack is empty");
    }

    // Check if the stack is empty
    bool isEmpty() const {
        return stack.empty();
    }
};

// Struct to represent a question
struct Question {
    string questionText;
    vector<string> opts;  // Options A, B, C, D
    int correctIndex;  // Index of the correct option
    string hint;  // Hint for the question
};

// Function to convert lifeline type to string
string lifelineName(LifelineType L) {
    switch (L) {
        case L50_50: return "50-50";
        case ASK_FRIEND: return "Ask a Friend";
        case SKIP: return "Skip";
        default: return "Unknown";
    }
}

// Function to read questions from a file
vector<Question> loadQuestionsFromFile(const string& filename) {
    vector<Question> questions;
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << "\n";
        return questions;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        
        // Read and ignore index and difficulty level (columns 1 and 2)
        getline(ss, token, '|');  // Index
        getline(ss, token, '|');  // Difficulty level
        
        // Read the question text (column 3)
        Question q;
        getline(ss, q.questionText, '|');
        
        // Read the options (columns 4-7)
        for (int i = 0; i < 4; i++) {
            getline(ss, token, '|');
            q.opts.push_back(token);
        }

        // Read the correct answer index (column 8)
        getline(ss, token, '|');
        q.correctIndex = stoi(token);

        // Read the hint (column 9)
        getline(ss, q.hint);

        // Add the question to the vector
        questions.push_back(q);
    }

    file.close();
    return questions;
}

// Function to ask for an answer, including using lifelines
int askForAnswer(const Question &q, myStack<LifelineType> &lifelines, bool &used50) {
    while (true) {
        cout << "Enter option (A-D), H for hint, L for lifelines: ";
        string in;
        getline(cin, in);
        if (in.empty()) continue;
        char c = toupper(in[0]);

        if (c >= 'A' && c <= 'D') {
            return c - 'A';  // Return the index of the selected option (0 to 3)
        } else if (c == 'H') {
            cout << "Hint: " << q.hint << "\n";  // Provide the hint
        } else if (c == 'L') {
            if (lifelines.isEmpty()) {  // Check if lifelines are available
                cout << "No lifelines left.\n";
                continue;
            }
            cout << "Available lifeline: " << lifelineName(lifelines.peek()) << " (will be consumed)\n";
            cout << "Use it? (Y/N): ";
            string y;
            getline(cin, y);
            if (!y.empty() && toupper(y[0]) == 'Y') {
                LifelineType L = lifelines.pop();  // Use the lifeline
                if (L == L50_50) {
                    // 50-50 lifeline: show the correct option and one wrong option
                    if (used50) {
                        cout << "50-50 already used earlier for this question.\n";
                    } else {
                        used50 = true;
                        cout << "Applying 50-50: remaining options:\n";
                        cout << char('A' + q.correctIndex) << ") " << q.opts[q.correctIndex] << "\n";
                        // pick a wrong option
                        vector<int> wrong;
                        for (int i = 0; i < 4; i++) if (i != q.correctIndex) wrong.push_back(i);
                        srand((unsigned)time(nullptr));
                        int pick = wrong[rand() % wrong.size()];
                        cout << char('A' + pick) << ") " << q.opts[pick] << "\n";
                    }
                } else if (L == ASK_FRIEND) {
                    // Ask Friend lifeline: simulate a friend's suggestion, biased toward the correct answer
                    srand((unsigned)time(nullptr));
                    int prob = rand() % 100;
                    if (prob < 70) {
                        cout << "Friend suggests: " << char('A' + q.correctIndex) << "\n";
                    } else {
                        // Wrong suggestion
                        vector<int> wrong;
                        for (int i = 0; i < 4; i++) if (i != q.correctIndex) wrong.push_back(i);
                        cout << "Friend suggests: " << char('A' + wrong[rand() % wrong.size()]) << "\n";
                    }
                } else if (L == SKIP) {
                    // Skip lifeline: automatically consider the question correct
                    cout << "Skip used: this question will be skipped and considered correct.\n";
                    return -2;  // special code for skip -> auto-correct
                }
            }
        } else if (c == 'S') {
            // Debug option to show the correct answer
            cout << "DEBUG correct: " << char('A' + q.correctIndex) << "\n";  // Debug only
        } else {
            cout << "Invalid input.\n";
        }
    }
}

// Function to simulate the game
void playGame(const string &playerName, const vector<Question>& questions) {
    // Create a lifelines stack with 3 lifelines (50-50, Ask a Friend, Skip)
    myStack<LifelineType> lifelines(3);
    lifelines.push(SKIP);
    lifelines.push(ASK_FRIEND);
    lifelines.push(L50_50);

    bool used50 = false;  // Track if the 50-50 lifeline has been used
    int score = 0;

    // Iterate through questions
    for (int i = 0; i < questions.size(); i++) {
        cout << "Question " << i + 1 << ": " << questions[i].questionText << "\n";
        for (const string &option : questions[i].opts) {
            cout << option << "\n";  // Display options
        }

        // Get the player's answer
        int answer = askForAnswer(questions[i], lifelines, used50);

        // Check the answer
        if (answer == -2) {
            cout << "You skipped this question.\n";
            score++;  // Skipped questions are treated as correct for the sake of the game
        } else if (answer == questions[i].correctIndex) {
            cout << "Correct answer!\n";
            score++;
        } else {
            cout << "Incorrect answer! The correct answer was: " << questions[i].opts[questions[i].correctIndex] << "\n";
        }
    }

    // Final score
    cout << "Game Over! Your final score: ";
}
//priority queue for difficulty scaling use max heap per TA recommendation




struct PlayerProfile {
    string name;
    int highScore = 0;
    int gamesPlayed = 0;
    int totalScore = 0;
};
const string QUESTIONS_FILE = "data/questions.txt";
const string PROFILES_FILE = "data/profiles.txt"; // simple text persistence

unordered_map<string, PlayerProfile> profiles; // hash table for player profiles


//leaderboard: array + merge sort (Ologn)
vector<PlayerProfile> mergeSorted(const vector<PlayerProfile>& a, const vector<PlayerProfile>& b) {
    vector<PlayerProfile> res;
    size_t i=0,j=0;
    while (i<a.size() && j<b.size()) {
        // sort by highScore descending, tie-breaker totalScore
        if (a[i].highScore > b[j].highScore) {
            res.push_back(a[i++]);
        } else if (a[i].highScore < b[j].highScore) {
            res.push_back(b[j++]);
        } else {
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
    return mergeSorted(left, right);
}


//file  opening 
struct Question {
    string id;
    int difficulty;         // 1..5
    string text;
    array<string,4> opts;
    int correctIndex;       // 0-based (0..3)
    string hint;
};
vector<string> splitPipe(const string &line) {
    // split on '|' (no support for escaped pipes in this simple parser)
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

vector<Question> loadQuestionsFromFile(const string &filename) {
    vector<Question> out;
    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Failed to open questions file: " << filename << "\n";
        return out;
    }
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        auto parts = splitPipe(line);
        // expected 9 parts: ID, diff, q, A,B,C,D, correctIndex, hint
        if (parts.size() < 9) continue;
        Question qq;
        qq.id = parts[0];
        qq.difficulty = stoi(parts[1]);
        qq.text = parts[2];
        qq.opts[0] = parts[3];
        qq.opts[1] = parts[4];
        qq.opts[2] = parts[5];
        qq.opts[3] = parts[6];
        qq.correctIndex = stoi(parts[7]);
        qq.hint = parts[8];
        out.push_back(qq);
    }
    in.close();
    return out;
}

};
