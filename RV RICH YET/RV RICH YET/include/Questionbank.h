#pragma once
#include "Question.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <map>

class QuestionBank {
public:
    QuestionBank() = default;
    // Load questions from file, returns true on success
    bool loadFromFile(const std::string& filename);

    // Return all loaded questions
    const std::vector<Question>& allQuestions() const;

    // Build convenience maps after load
    void buildMaps();

    // Optional: get question by id (fast O(1))
    bool getQuestionById(const std::string& id, Question &out) const;

    // Get questions by difficulty
    const std::vector<Question>& getQuestionsByDifficulty(int difficulty) const;

    // Build a priority queue externally: we expose vector of PQItems (or just use allQuestions())
private:
    std::vector<Question> questions_;
    std::unordered_map<std::string, Question> questionById_;
    std::unordered_map<std::string, std::vector<std::string>> categoryToIds_;
    std::map<int, std::vector<Question>> categorized_questions;
};
