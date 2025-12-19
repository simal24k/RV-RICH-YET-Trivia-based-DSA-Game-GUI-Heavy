#include "data_structures.hpp"


PrizeLadder::PrizeLadder() : head(nullptr), current(nullptr), totalLevels(0)
{
    initializeLadder();
}

PrizeLadder::~PrizeLadder()
{
    PrizeNode *temp = head;
    while (temp)
    {
        PrizeNode *next = temp->next;
        delete temp;
        temp = next;
    }
}

void PrizeLadder::initializeLadder()
{
    long long prizes[] = {0, 100, 200, 300, 500, 1000, 2000, 4000, 8000, 16000, 32000, 64000, 125000, 250000, 500000, 1000000};

    
    bool safetyLevels[] = {true, false, false, false, false, true, false, false, false, false, true, false, false, false, false, true};

    for (int i = 0; i < 16; i++)
    {
        PrizeNode *newNode = new PrizeNode(i, prizes[i], safetyLevels[i]);
        if (!head)
        {
            head = newNode;
            current = head;
        }
        else
        {
            PrizeNode *temp = head;
            while (temp->next)
                temp = temp->next;
            temp->next = newNode;
            newNode->prev = temp;
        }
        totalLevels++;
    }
}

void PrizeLadder::moveForward()
{
    if (current && current->next)
    {
        current = current->next;
    }
}

void PrizeLadder::moveToSafetyLevel()
{
    if (current && current->prev)
    {
        PrizeNode *temp = current->prev;
        
        while (temp && !temp->isSafetyLevel)
        {
            temp = temp->prev;
        }
       
        if (temp)
            current = temp;
       
        else
            current = head;
    }
}

void PrizeLadder::resetLadder()
{
    if (head)
    {
        current = head;
    }
}

long long PrizeLadder::getCurrentPrize() const
{
    return current ? current->prizeAmount : 0;
}

int PrizeLadder::getCurrentLevel() const
{
    return current ? current->level : 0;
}

bool PrizeLadder::isSafetyLevel() const
{
    return current ? current->isSafetyLevel : false;
}

PrizeNode *PrizeLadder::getHead() const { return head; }
PrizeNode *PrizeLadder::getCurrent() const { return current; }

// Player Implementation
Player::Player() : totalWinnings(0), questionsAnswered(0), currentLevel(0)
{
    lifelinesUsed[0] = 0;
    lifelinesUsed[1] = 0;
    lifelinesUsed[2] = 0;
    lifelinesUsed[3] = 0;
}

void Player::recordQuestion(int qID)
{
    questionsAsked[qID] = true;
}

bool Player::hasAskedQuestion(int qID) const
{
    return questionsAsked.find(qID) != questionsAsked.end();
}

// LifelineStack Implementation
void LifelineStack::usedLifeline(int type)
{
    lifelineHistory.push(type);
}

bool LifelineStack::canUseLifeline(int type, const Player &player) const
{
    return player.lifelinesUsed[type] == 0;
}

int LifelineStack::getLifelineCount() const
{
    return lifelineHistory.size();
}


void QuestionPriorityQueue::bubbleUp(int index)
{
    while (index > 0 && heap[(index - 1) / 2].category < heap[index].category)
    {
        swap(heap[index], heap[(index - 1) / 2]);
        index = (index - 1) / 2;
    }
}

void QuestionPriorityQueue::bubbleDown(int index)
{
    int size = heap.size();
    while (2 * index + 1 < size)
    {
        int larger = 2 * index + 1;
        if (2 * index + 2 < size && heap[2 * index + 2].category > heap[larger].category)
        {
            larger = 2 * index + 2;
        }
        if (heap[index].category < heap[larger].category)
        {
            swap(heap[index], heap[larger]);
            index = larger;
        }
        else
        {
            break;
        }
    }
}

void QuestionPriorityQueue::insert(const Question &q)
{
    heap.push_back(q);
    bubbleUp(heap.size() - 1);
}

Question QuestionPriorityQueue::extractMax()
{
    Question max = heap[0];
    heap[0] = heap.back();
    heap.pop_back();
    if (!heap.empty())
        bubbleDown(0);
    return max;
}

bool QuestionPriorityQueue::isEmpty() const { return heap.empty(); }