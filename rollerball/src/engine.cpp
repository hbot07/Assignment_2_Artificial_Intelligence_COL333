#include <algorithm>
#include <random>
#include <iostream>
#include <vector>
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
    this->best_move = minimax(*n,7,-1,1).first;
    std::cout << "exiting with move" << best_move<< "\n";
}

int Engine::find_score( Board& b){

short score = b.static_score;

if(b.data.player_to_play == BLACK){
        score += int(0.3*b.num_moves);
    }
    else{
        score -= int(0.3*b.num_moves);
    }

if(b.in_check()){
    if(b.data.player_to_play == BLACK){
        score += 2;
    }
    else{
        score -= 2;
    }

}

return score;
}

bool Engine::compare_for_order_Black(const std::pair<short,U16> &m1,const std::pair<short,U16>&m2){
        return m1.first <= m2.first;
}

bool Engine::compare_for_order_White(const std::pair<short,U16> &m1,const std::pair<short,U16>&m2){
        return m1.first >=m2.first;
}
void Engine::order_Nodes(Board& b,int player,std::unordered_set<U16> *moveset,std::vector<std::pair<short,U16>> *aux){
for(auto m:(*moveset)){
    b.do_move(m);
    if(player==BLACK){
    aux->push_back(std::make_pair((this->find_score(b)),m));
    }
    else{
      aux->push_back(std::make_pair(-1*(this->find_score(b)),m));
    }
    b.undo_move(m);
}
//else{std::sort(aux->begin(),aux->end(),compare_for_order_White);}
    
    std::sort(aux->begin(),aux->end());
}

std::pair<int,int> Engine::minimax(Board& b,int depth_of_search,int alpha,int beta){

if (depth_of_search == 0){
     if(b.in_check()){
        return minimax(b,2,alpha,beta);
}
    return std::make_pair(0,this->find_score(b));
}
auto moveset = b.get_legal_moves();
    if (moveset.size() == 0) {

        if (b.data.player_to_play == BLACK){
        return std::make_pair(0,35);
        }
        else{
            return std::make_pair(0,-35);
        }
    }
std::vector<std::pair<short,U16>>  auxillary;
this->order_Nodes(b,b.data.player_to_play,&moveset,&auxillary);
//std::random_shuffle(moveset.begin(),moveset.end());
int auxill;
if(b.data.player_to_play== BLACK){
    auxill = int((-0.2)* moveset.size());
}
else{
    auxill = int((0.2)*moveset.size());
}
int best_score = -1;
std::pair<int,int> rvalue;
for(long unsigned int x = 0;x < auxillary.size();x++ ){
    if(alpha >= beta && alpha != -1 && beta != 1){
        break;
    }
    b.do_move(auxillary[x].second);
    
    auxill += minimax(b,depth_of_search - 1,alpha,beta).second;
    if (b.data.player_to_play == BLACK){
    if (auxill > best_score || best_score == -1){
        best_score = auxill;
       rvalue.first = auxillary[x].second;
       rvalue.second = best_score;
    }
    if (auxill > alpha||alpha == -1){
        alpha = auxill;
    }
    auxill = int((-0.2)*moveset.size());
    }
    else{
      if (auxill < best_score || best_score == -1){
        best_score = auxill;
       rvalue.first = auxillary[x].second;
       rvalue.second = best_score;
    }
    if(auxill < beta || beta == 1){
        beta = auxill;
    }  
    auxill = int((0.2)*moveset.size());
    }
    b.undo_move(auxillary[x].second);
    
}
auxillary.clear();
return rvalue;
}