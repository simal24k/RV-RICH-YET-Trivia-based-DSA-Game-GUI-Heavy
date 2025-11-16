// include/Question.h

#ifndef QUESTION_H
#define QUESTION_H

#include <string>
#include <array>
#include <iostream>

class Question {
private:
    // All members from your original struct definition
    std::string id;
    std::string category; 
    int difficulty;   // 1-15
    std::string text;
    std::array<std::string, 4> options; // Uses std::array as specified
    int correctIndex; // Index 0-3 of the correct option
    std::string hint;

public:
    // Full Constructor: Matches the full set of members
    Question(const std::string& qId, const std::string& cat, int diff, 
             const std::string& t, const std::array<std::string, 4>& opts, 
             int cI, const std::string& h);

    // Accessors
    const std::string& getId() const;       // CRITICAL for ProfileManager (Hash Set)
    const std::string& getCategory() const;
    int getDifficulty() const;
    const std::string& getText() const;
    const std::array<std::string, 4>& getOptions() const;
    int getCorrectIndex() const;
    const std::string& getHint() const;

    void display() const;
};

#endif // QUESTION_H