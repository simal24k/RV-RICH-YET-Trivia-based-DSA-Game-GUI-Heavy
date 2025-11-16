#include "Lifelines.h"

std::string lifelineName(LifelineType type) {
    switch (type) {
        case LifelineType::FiftyFifty: return "50-50";
        case LifelineType::AskFriend:  return "Ask a Friend";
        case LifelineType::Skip:       return "Skip";
    }
    return "Unknown";
}

Lifelines::Lifelines() {
    // Stack order (top = last to be used)
    st_.push(LifelineType::Skip);
    st_.push(LifelineType::AskFriend);
    st_.push(LifelineType::FiftyFifty);
}

bool Lifelines::hasAny() const {
    return !st_.empty();
}

LifelineType Lifelines::top() const {
    return st_.top();
}

void Lifelines::pop() {
    st_.pop();
}
