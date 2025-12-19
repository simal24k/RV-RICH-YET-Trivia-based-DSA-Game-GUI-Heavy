#include "question_bank.hpp"
#include <algorithm> 
#include <random>    

using namespace std;

void CategoryNetwork::addQuestionToCategory(int categoryId, int questionId)
{
    categoryMap[categoryId].push_back(questionId);
}

vector<int> CategoryNetwork::getQuestionsForCategory(int categoryId) const
{
    auto it = categoryMap.find(categoryId);
    if (it != categoryMap.end())
    {
        return it->second;
    }
    return vector<int>();
}

void CategoryNetwork::displayCategoryInfo() const
{
    cout << "\n========== CATEGORY NETWORK ==========\n";
    for (const auto &entry : categoryMap)
    {
        cout << "Category " << entry.first << ": " << entry.second.size() << " questions\n";
    }
    cout << "======================================\n";
}

QuestionBank::QuestionBank() {}

bool QuestionBank::loadFromFile(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Could not open " << filename << endl;
        return false;
    }

    // RNG for shuffling options
    unsigned seed = (unsigned)time(0);
    default_random_engine rng(seed);

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

        if (tokens.size() < 9)
            continue;

        Question q;
        q.id = stoi(tokens[0]);
        q.category = stoi(tokens[1]);
        q.text = tokens[2];

        // Load original options
        vector<string> rawOptions = {tokens[3], tokens[4], tokens[5], tokens[6]};
        int originalCorrectIndex = stoi(tokens[7]);

        // FIX: Shuffle logic
        // 1. Identify the correct answer string
        string correctAnswer = rawOptions[originalCorrectIndex];

        // 2. Shuffle the options vector
        q.options = rawOptions;
        shuffle(q.options.begin(), q.options.end(), rng);

        // 3. Find the new index of the correct answer
        for (int i = 0; i < 4; i++)
        {
            if (q.options[i] == correctAnswer)
            {
                q.correctAnswerIndex = i;
                break;
            }
        }

        q.hint = tokens[8];

        questions.push_back(q);
        answers[q.id] = q.options[q.correctAnswerIndex];

        categoryNetwork.addQuestionToCategory(q.category, q.id);
    }

    file.close();
    shuffleQuestions();
    return true;
}

void QuestionBank::shuffleQuestions()
{
    srand((unsigned)time(0));
    for (int i = questions.size() - 1; i > 0; --i)
    {
        int j = rand() % (i + 1);
        swap(questions[i], questions[j]);
    }
}

Question QuestionBank::getNextQuestion(const Player &player)
{
    for (const auto &q : questions)
    {
        if (!player.hasAskedQuestion(q.id))
        {
            return q;
        }
    }
    return Question{-1, 0, "No more questions", {}, -1, ""};
}

const string &QuestionBank::getCorrectAnswer(int questionID) const
{
    return answers.at(questionID);
}

bool QuestionBank::isCorrectAnswer(int questionID, int optionIndex) const
{
    if (questions.empty())
        return false;
    for (const auto &q : questions)
    {
        if (q.id == questionID)
        {
            return q.correctAnswerIndex == optionIndex;
        }
    }
    return false;
}

int QuestionBank::getTotalQuestions() const { return questions.size(); }

const vector<Question> &QuestionBank::getAllQuestions() const { return questions; }

string QuestionBank::getCategoryName(int categoryId) const
{
    switch (categoryId)
    {
    case 0:
        return "ADT & Data Structures";
    case 1:
        return "Linked Lists";
    case 2:
        return "Stacks & Queues";
    case 3:
        return "Trees";
    case 4:
        return "Graphs";
    case 5:
        return "Hashing / Advanced Topics";
    default:
        return "Unknown";
    }
}