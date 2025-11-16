#pragma once
#include <string>
#include <stack>

enum class LifelineType {
    FiftyFifty,
    AskFriend,
    Skip
};

std::string lifelineName(LifelineType type);

class Lifelines {
public:
    Lifelines();

    bool hasAny() const;
    LifelineType top() const;
    void pop();

private:
    std::stack<LifelineType> st_;
};
