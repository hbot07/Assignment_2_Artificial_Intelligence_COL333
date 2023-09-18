#pragma once

#include "board.hpp"
#include <atomic>

class Engine {

    public:
    std::atomic<U16> best_move;
    std::atomic<bool> search;
    int minimax(const Board &b, int depth, bool isMaximizingPlayer, int alpha, int beta);
    virtual void find_best_move(const Board& b);
};
