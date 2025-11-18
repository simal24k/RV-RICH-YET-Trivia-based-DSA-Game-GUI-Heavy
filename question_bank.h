#ifndef QUESTION_BANK_H
#define QUESTION_BANK_H

#include "data_structures.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
using namespace std;
class QuestionBank {
private:
     vector<Question> questions;
     unordered_map<int,  string> answers; // ID -> correct answer
     vector<Question> usedQuestions;

public:
    QuestionBank() {}

    bool loadFromFile(const  string& filename) {
         ifstream file(filename);
        if (!file.is_open()) {
             cerr << "Error: Could not open " << filename <<  endl;
            return false;
        }

         string line;
        while ( getline(file, line)) {
            if (line.empty()) continue;
            
             vector< string> tokens;
             stringstream ss(line);
             string token;
            
            while ( getline(ss, token, '|')) {
                tokens.push_back(token);
            }

            if (tokens.size() < 9) continue;

            Question q;
            q.id =  stoi(tokens[0]);
            q.category =  stoi(tokens[1]);
            q.text = tokens[2];
            q.options = {tokens[3], tokens[4], tokens[5], tokens[6]};
            q.correctAnswerIndex =  stoi(tokens[7]);
            q.hint = tokens[8];

            questions.push_back(q);
            answers[q.id] = q.options[q.correctAnswerIndex];
        }

        file.close();
        shuffleQuestions();
        return true;
    }

    void shuffleQuestions() {
         srand( time(0));
        for (int i = questions.size() - 1; i > 0; --i) {
            int j =  rand() % (i + 1);
             swap(questions[i], questions[j]);
        }
    }

    Question getNextQuestion(const Player& player) {
        for (const auto& q : questions) {
            if (!player.hasAskedQuestion(q.id)) {
                return q;
            }
        }
        return Question{-1, 0, "No more questions", {}, -1, ""};
    }

    const  string& getCorrectAnswer(int questionID) const {
        return answers.at(questionID);
    }

    bool isCorrectAnswer(int questionID, int optionIndex) const {
        if (questions.empty()) return false;
        for (const auto& q : questions) {
            if (q.id == questionID) {
                return q.correctAnswerIndex == optionIndex;
            }
        }
        return false;
    }

    int getTotalQuestions() const { return questions.size(); }

    const  vector<Question>& getAllQuestions() const { return questions; }
};

#endif
