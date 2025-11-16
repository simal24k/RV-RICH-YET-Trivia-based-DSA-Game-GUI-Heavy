// src/Question.cpp

#include "Question.h"
#include <iostream>
#include <iomanip>

// Full 7-argument constructor implementation
Question::Question(const std::string& qId, const std::string& cat, int diff, 
                   const std::string& t, const std::array<std::string, 4>& opts, 
                   int cI, const std::string& h)
    : id(qId), category(cat), difficulty(diff), text(t), options(opts), 
      correctIndex(cI), hint(h) {}

// Accessor implementations
const std::string& Question::getId() const {
    return id;
}

const std::string& Question::getCategory() const {
    return category;
}

int Question::getDifficulty() const {
    return difficulty;
}

const std::string& Question::getText() const {
    return text;
}

const std::array<std::string, 4>& Question::getOptions() const {
    return options;
}

int Question::getCorrectIndex() const {
    return correctIndex;
}

const std::string& Question::getHint() const {
    return hint;
}

// Display implementation
void Question::display() const {
    std::cout << "\n[Level " << difficulty << " - " << category << "]\n";
    std::cout << "Question: " << text << "\n";
    
    // Display options A, B, C, D
    const char* labels = "ABCD";
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << std::setw(3) << labels[i] << ". " << options[i] << "\n";
    }
}