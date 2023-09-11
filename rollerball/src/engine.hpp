#pragma once

#include "board.hpp"
#include <atomic>

class Engine {

    public:
    std::atomic<U16> best_move;
    std::atomic<bool> search;
    int find_score(Board&b);
    std::pair<int,int> minimax(Board&b,int depth_of_search);
    virtual void find_best_move(const Board& b);
};
