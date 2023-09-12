#include <algorithm>
#include <random>
#include <iostream>

#include "board.hpp"
#include "engine.hpp"

#define pos(x,y) (((y)<<3)|(x))
#define DEAD pos(7,7)
#define WHITE (1<<6)
#define BLACK (1<<5)
void Engine::find_best_move(const Board& b) {

    // pick a random move
    
    // auto moveset = b.get_legal_moves();
    // if (moveset.size() == 0) {
    //     this->best_move = 0;
    // }
    // else {
    //     std::vector<U16> moves;
    //     std::cout << all_boards_to_str(b) << std::endl;
    //     for (auto m : moveset) {
    //         std::cout << move_to_str(m) << " ";
    //     }
    //     std::cout << std::endl;
    //     std::sample(
    //         moveset.begin(),
    //         moveset.end(),
    //         std::back_inserter(moves),
    //         1,
    //         std::mt19937{std::random_device{}()}
    //     );
    //     this->best_move = moves[0];
    // }
    this->best_move = 0;
    Board* n = b.copy();
    this->best_move = minimax(*n,5,-1,1).first;
    std::cout << "exiting with move" << best_move<< "\n";
}

int Engine::find_score( Board& b){

int score = 0;
if(b.data.b_bishop == DEAD) score += 50;
if(b.data.b_king == DEAD) score += 50;
if(b.data.b_pawn_bs == DEAD) score += 10;
if(b.data.b_pawn_ws == DEAD) score += 10;
if(b.data.b_rook_bs == DEAD) score += 30;
if(b.data.b_rook_ws == DEAD) score += 30;

if(b.data.w_bishop == DEAD) score -= 50;
if(b.data.w_king == DEAD) score -= 50;
if(b.data.w_pawn_bs == DEAD) score -= 10;
if(b.data.w_pawn_ws == DEAD) score -= 10;
if(b.data.w_rook_bs == DEAD) score -= 30;
if(b.data.w_rook_ws == DEAD) score -= 30;

if(b.in_check()){
    if(b.data.player_to_play == BLACK){
        score += 100;
    }
    else{
        score -= 100;
    }

}

return score;
}

std::pair<int,int> Engine::minimax(Board& b,int depth_of_search,int alpha,int beta){

if (depth_of_search == 0){

    return std::make_pair(0,find_score(b));
}
auto moveset = b.get_legal_moves();
    if (moveset.size() == 0) {

        if (b.data.player_to_play == BLACK){
        return std::make_pair(0,300);
        }
        else{
            return std::make_pair(0,-300);
        }
    }
//std::random_shuffle(moveset.begin(),moveset.end());
int auxillary;
int best_score = -1;
std::pair<int,int> rvalue;
for(auto m:moveset){
    if(alpha >= beta && alpha != -1 && beta != 1){
        break;
    }
    if(search ==false){
        std::cout << "exiting function"<< best_move<< "\n";

        return rvalue;
    }
    b.do_move(m);
    
    auxillary = minimax(b,depth_of_search - 1,alpha,beta).second;
    if (b.data.player_to_play == BLACK){
    if (auxillary > best_score || best_score == -1){
        best_score = auxillary;
       rvalue.first = m;
       rvalue.second = best_score;
    }
    if (auxillary > alpha||alpha == -1){
        alpha = auxillary;
    }
    }
    else{
      if (auxillary < best_score || best_score == -1){
        best_score = auxillary;
       rvalue.first = m;
       rvalue.second = best_score;
    }
    if(auxillary < beta || beta == 1){
        beta = auxillary;
    }  
    }
    b.undo_move(m);
     if(search ==false){
        std::cout << "exiting function"<< best_move<< "\n";

        return rvalue;
    }
    
}
return rvalue;
}