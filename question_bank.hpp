#ifndef QUESTION_BANK_HPP
#define QUESTION_BANK_HPP

#include "data_structures.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace std;

class CategoryNetwork {
private:
    unordered_map<int, vector<int>> categoryMap; // category -> list of question IDs
    
public:
    void addQuestionToCategory(int categoryId, int questionId);
    vector<int> getQuestionsForCategory(int categoryId) const;
    void displayCategoryInfo() const;
};

class QuestionBank {
private:
    vector<Question> questions;
    unordered_map<int, string> answers; // ID -> correct answer
    vector<Question> usedQuestions;
    CategoryNetwork categoryNetwork; // Category network instance

public:
    QuestionBank();
    
    bool loadFromFile(const string& filename);
    void shuffleQuestions();
    Question getNextQuestion(const Player& player);
    const string& getCorrectAnswer(int questionID) const;
    bool isCorrectAnswer(int questionID, int optionIndex) const;
    int getTotalQuestions() const;
    const vector<Question>& getAllQuestions() const;
    string getCategoryName(int categoryId) const; // New method for category lookup
    CategoryNetwork& getCategoryNetwork() { return categoryNetwork; } // Access category network
};

#endif
