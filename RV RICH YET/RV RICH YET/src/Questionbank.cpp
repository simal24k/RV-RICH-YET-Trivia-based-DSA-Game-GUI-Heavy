// src/QuestionBank.cpp - FINAL ROBUST PARSER FIX

#include "Questionbank.h"
#include "Question.h" 
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <array>
#include <cctype> // Required for whitespace check

// Helper to trim leading/trailing whitespace
std::string trim(const std::string& str) {
    auto is_not_space = [](int c){ return !std::isspace(c); };

    // Find first non-space character
    size_t first = std::find_if(str.begin(), str.end(), is_not_space) - str.begin();
    if (first == str.length()) { // Entire string is whitespace
        return "";
    }

    // Find last non-space character
    size_t last = std::find_if(str.rbegin(), str.rend(), is_not_space).base() - str.begin();

    return str.substr(first, last - first);
}

// =========================================================================
// Helper Function: Split a string by a delimiter (uses trim for robustness)
// =========================================================================
std::vector<std::string> splitPipe(const std::string& s, char delimiter = '|') {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        // CRITICAL FIX: Trim whitespace from each token before storing
        tokens.push_back(trim(token));
    }
    return tokens;
}

// =========================================================================
// loadFromFile Implementation 
// =========================================================================
bool QuestionBank::loadFromFile(const std::string& filePath) {
    questions_.clear();
    categorized_questions.clear();

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open question file at " << filePath << std::endl;
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::vector<std::string> all_parts = splitPipe(content);

    for (size_t i = 0; i + 8 < all_parts.size(); i += 9) {
        std::vector<std::string> parts(all_parts.begin() + i, all_parts.begin() + i + 9);

        try {
            // Extract ALL 9 fields
            std::string qId = parts[0];
            int difficulty = std::stoi(parts[1]);
            std::string qText = parts[2];

            std::array<std::string, 4> opts_array = {
                parts[3], parts[4], parts[5], parts[6]
            };

            int correctIndex = std::stoi(parts[7]);
            std::string hint = parts[8];

            std::string category = "DSA Fundamentals";

            Question new_q(qId, category, difficulty, qText, opts_array, correctIndex, hint);
            questions_.push_back(new_q);

        } catch (const std::exception& e) {
            std::cerr << "Error parsing question data at chunk " << i / 9 << ": " << e.what() << std::endl;
            continue;
        }
    }

    return !questions_.empty();
}

void QuestionBank::buildMaps() {
    categorized_questions.clear();
    // DSA Usage: Hash Map Insertion (A)
    for (const auto& q : questions_) {
        int diff = q.getDifficulty();
        categorized_questions[diff].push_back(q);
    }
}

// ... Accessor Methods remain unchanged ...
const std::vector<Question>& QuestionBank::allQuestions() const {
    return questions_;
}

const std::vector<Question>& QuestionBank::getQuestionsByDifficulty(int difficulty) const {
    static const std::vector<Question> empty_vector;
    auto it = categorized_questions.find(difficulty);
    if (it != categorized_questions.end()) {
        return it->second;
    }
    return empty_vector;
}