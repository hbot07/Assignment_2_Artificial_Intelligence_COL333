#pragma once

#include "board.hpp"
#include <atomic>

class Engine {

    public:
    std::atomic<U16> best_move;
    std::atomic<bool> search;
    int find_score(Board&b);
    std::pair<int,int> minimax(Board&b,int depth_of_search,int alpha,int beta);
    virtual void find_best_move(const Board& b);
    static bool compare_for_order_Black(const std::pair<short,U16> &m1,const std::pair<short,U16>&m2);
    static bool compare_for_order_White(const std::pair<short,U16> &m1,const std::pair<short,U16>&m2);
    virtual void order_Nodes(Board& b,int player,std::unordered_set<U16>*moveset,std::vector<std::pair<short,U16>> *aux);
};
