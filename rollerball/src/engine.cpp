#include <algorithm>
#include <random>
#include <iostream>


#include <chrono>

#include "board.hpp"
#include "engine.hpp"

// Define constant for positive and negative infinity
const int INF = 1e9;
const int NEG_INF = -INF;

// Define a helper function to evaluate the board

// NOTE: This is a dummy function and should be replaced with an appropriate evaluation function for your game.
// Piece values
const short PAWN_VALUE   = 15;
const short ROOK_VALUE   = 100;
const short KING_VALUE   = 900; // This might be arbitrary since the game is over if the King is captured, but for evaluation purposes, it has a high value.
const short BISHOP_VALUE = 55;

// Piece square tables
// Note: These are placeholders. For an optimal evaluation, the values inside should be tuned based on the specifics of the game.
// Placeholder values for piece square tables.

constexpr U8 cw_90[64] = {
    48, 40, 32, 24, 16, 8,  0,  7,
    49, 41, 33, 25, 17, 9,  1,  15,
    50, 42, 18, 19, 20, 10, 2,  23,
    51, 43, 26, 27, 28, 11, 3,  31,
    52, 44, 34, 35, 36, 12, 4,  39,
    53, 45, 37, 29, 21, 13, 5,  47,
    54, 46, 38, 30, 22, 14, 6,  55,
    56, 57, 58, 59, 60, 61, 62, 63
};

constexpr U8 acw_90[64] = {
     6, 14, 22, 30, 38, 46, 54, 7,
     5, 13, 21, 29, 37, 45, 53, 15,
     4, 12, 18, 19, 20, 44, 52, 23,
     3, 11, 26, 27, 28, 43, 51, 31,
     2, 10, 34, 35, 36, 42, 50, 39,
     1,  9, 17, 25, 33, 41, 49, 47,
     0,  8, 16, 24, 32, 40, 48, 55,
    56, 57, 58, 59, 60, 61, 62, 63
};

constexpr U8 cw_180[64] = {
    54, 53, 52, 51, 50, 49, 48, 7,
    46, 45, 44, 43, 42, 41, 40, 15,
    38, 37, 18, 19, 20, 33, 32, 23,
    30, 29, 26, 27, 28, 25, 24, 31,
    22, 21, 34, 35, 36, 17, 16, 39,
    14, 13, 12, 11, 10,  9,  8, 47,
     6,  5,  4,  3,  2,  1,  0, 55,
    56, 57, 58, 59, 60, 61, 62, 63
};

constexpr U8 id[64] = {
     0,  1,  2,  3,  4,  5,  6,  7,
     8,  9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61, 62, 63
};

const short PAWN_SQUARE_TABLE[64] = {
       0, 1, 2, 3, 5, 0, 0, 0,
        0, 1, 2, 3, 5, 5, 5, 5,
        1, 5, 2, 3, 3, 2, 1, 1,
        1, 4, 1, 2, 2, 1, 0, 0,
        1, 3, 0, 0, 0, 0, 0, 0,
        1, 3, 4, 0, -2, 1, 1, 1,
        1, 2, 1, 0, -4, 2, 2, 1,
        0, 0, 0, 0, 0, 0, 0, 0
};

const short ROOK_SQUARE_TABLE[64] = {
        22, 22, 25,25 , 25, 30, 30, 0,
        25,10 , 10, 10, 10, 11, 11, 5,
        20, 12, 10, 10, 10, 8, 8, 5,
        16, 11, 10, 10, 10, 5, 5, 5,
        16, 15, 10, 10, 10, 5, 5, 5,
        20,17, 10, 13, 0, 15, 13, 5,
        15, 8, 2, 3, 0, 4, 5, 5,
        0, 0, 0, 5, 5, 0, 0, 0
};

const short KING_SQUARE_TABLE[64] = {
      -30, -20, 0, 0, 0, -5, -30, 20,
        -20, 20, 15, 0, 15, 0, -10, 20,
        -20, 15, -20, -20, -20, 15, -20, -10,
        -30, -10, -20, -20, -20, -20, -30, -10,
        -20, 15, -30, -40, 5, 7, -30, -20,
        -20, -5, 15, 0, -10, -20, -40, -30,
        -50, -20, -20, -30, -20, -30, -20, -30,
        0, 0, 0, 0, 0, 0, 0, 0
};

const short BISHOP_SQUARE_TABLE[64] = {
        7, 10,10, 25, 10, 6, 7, 0,
        10, 10, 20, 10, 20, 3, 6, 5,
        10, 15, 10, 10, 10, 15, 4, 5,
        20, 10, 10, 10, 10, 3, 20, 5,
        10, 15, 10, 10, 10, 15, 4, 5,
        10, 10, 15, 6, 15, 15, 3, 4,
        7, 10, 10, 20, 10, 3, 7, 5,
        0, 0, 0, 5, 5, 0, 0, 0
};

int evaluate(const Board &b) {
    int score = 0;

    // Material count
    for (int i = 0; i < 7; i++) {
        for (int j = 0;j < 7;j++){
            if(i >=2 && i <= 4 && j >= 2 &&j<= 4){
                continue;
            }
        U8 piece = b.data.board_0[8*i + j];
        switch (piece & ~PlayerColor::WHITE & ~PlayerColor::BLACK) {
            case PieceType::PAWN:
                score += (piece & PlayerColor::WHITE) ? PAWN_VALUE + PAWN_SQUARE_TABLE[8*(6-i) + j] : -PAWN_VALUE - PAWN_SQUARE_TABLE[8*i +6- j];
                break;
            case PieceType::ROOK:
                score += (piece & PlayerColor::WHITE) ? ROOK_VALUE + ROOK_SQUARE_TABLE[8*(6-i) + j] : -ROOK_VALUE - ROOK_SQUARE_TABLE[8*i +6-j];
                break;
            case PieceType::KING:
                score += (piece & PlayerColor::WHITE) ? KING_VALUE + KING_SQUARE_TABLE[8*(6-i) + j] : -KING_VALUE - KING_SQUARE_TABLE[8*i +6-j];
                break;
            case PieceType::BISHOP:
                score += (piece & PlayerColor::WHITE) ? BISHOP_VALUE + BISHOP_SQUARE_TABLE[8*(6-i) + j] : -BISHOP_VALUE - BISHOP_SQUARE_TABLE[8*i +6-j];
                break;
        }}
    }

    // Check and Checkmate
    // if (b.in_check()) {
    //     score += (b.data.player_to_play == PlayerColor::WHITE) ? -200 : 200;  // If white is in check, subtract value. If black is in check, add value.
    // }
    // Note: Check for checkmate can be added as well but you haven't provided a function to determine checkmate.
    // For instance:
    // if (b.is_checkmate()) {
    //     score += (b.data.player_to_play == PlayerColor::WHITE) ? -1000 : 1000;
    // }

    return score;
}


int get_added_score(U16 move,const Board &b){
    U8 p0 = getp0(move);
    U8 y0  = gety(p0);
    U8 x0  = getx(p0);
    U8 p1 = getp1(move);
    U8 piecetype = b.data.board_0[p1];
    U8 y1  = gety(p1);
    U8 x1  = getx(p1);
     p1 = 8*(6-y1) + x1;
     p0 = 8*(6-y0) + x0;
     U8 promo = getpromo(move);
    
    U8 deadpiece = b.data.last_killed_piece;
    int added = 0;
    switch (piecetype & ~PlayerColor::WHITE & ~PlayerColor::BLACK) {
            case PieceType::PAWN:
                added += (piecetype & PlayerColor::WHITE) ? PAWN_SQUARE_TABLE[p1] -  PAWN_SQUARE_TABLE[p0] : PAWN_SQUARE_TABLE[p0] - PAWN_SQUARE_TABLE[p1];
                if (promo == PAWN_ROOK){
                     added += (piecetype & PlayerColor::WHITE) ? ROOK_VALUE + ROOK_SQUARE_TABLE[p1]- PAWN_VALUE - PAWN_SQUARE_TABLE[p1] : PAWN_SQUARE_TABLE[p1]-ROOK_VALUE - ROOK_SQUARE_TABLE[p1]+PAWN_VALUE;
                     break;
                } 
                if(promo == PAWN_BISHOP){
                    added += (piecetype & PlayerColor::WHITE) ? BISHOP_VALUE + BISHOP_SQUARE_TABLE[p1]- PAWN_VALUE - PAWN_SQUARE_TABLE[p1]:PAWN_SQUARE_TABLE[p1]-BISHOP_VALUE - BISHOP_SQUARE_TABLE[p1]+PAWN_VALUE;
                    break;
                }
                break;
            case PieceType::ROOK:
                added += (piecetype & PlayerColor::WHITE) ? ROOK_SQUARE_TABLE[p1] - ROOK_SQUARE_TABLE[p0]: ROOK_SQUARE_TABLE[p0]- ROOK_SQUARE_TABLE[p1];
                break;
            case PieceType::KING:
                added += (piecetype & PlayerColor::WHITE) ? KING_SQUARE_TABLE[p1] - KING_SQUARE_TABLE[p0] : KING_SQUARE_TABLE[p0] - KING_SQUARE_TABLE[p1];
                break;
            case PieceType::BISHOP:
                added += (piecetype & PlayerColor::WHITE) ?  BISHOP_SQUARE_TABLE[p1] - BISHOP_SQUARE_TABLE[p0]: BISHOP_SQUARE_TABLE[p0] - BISHOP_SQUARE_TABLE[ p1];
                break;
        }
    switch (deadpiece & ~PlayerColor::WHITE & ~PlayerColor::BLACK) {
            case PieceType::PAWN:
                added -= (deadpiece & PlayerColor::WHITE) ? PAWN_VALUE + PAWN_SQUARE_TABLE[p1] :  -PAWN_VALUE - PAWN_SQUARE_TABLE[p1];
                break;
            case PieceType::ROOK:
                added -= (deadpiece & PlayerColor::WHITE) ? ROOK_VALUE + ROOK_SQUARE_TABLE[p1] : -ROOK_VALUE - ROOK_SQUARE_TABLE[p1];
                break;
            case PieceType::KING:
                added -= (deadpiece & PlayerColor::WHITE) ? KING_VALUE + KING_SQUARE_TABLE[p1] :-KING_VALUE - KING_SQUARE_TABLE[p1];
                break;
            case PieceType::BISHOP:
                added -= (deadpiece & PlayerColor::WHITE) ? BISHOP_VALUE + BISHOP_SQUARE_TABLE[p1]:-BISHOP_VALUE - BISHOP_SQUARE_TABLE[p1];
                break;
        }
//  if (b.in_check()) {
//         added += (b.data.player_to_play == PlayerColor::BLACK) ? 200:-200;  // If white is in check, subtract value. If black is in check, add value.
//     }

return added;
}
void _flip_player( Board &b) {
    b.data.player_to_play = (PlayerColor)(b.data.player_to_play ^ (WHITE | BLACK));
}

void _undo_last_move(U16 move, Board &b) {

    U8 p0 = getp0(move);
    U8 p1 = getp1(move);
    U8 promo = getpromo(move);

    U8 piecetype = b.data.board_0[p1];
    U8 deadpiece = b.data.last_killed_piece;
    b.data.last_killed_piece = 0;

    // scan and get piece from coord
    U8 *pieces = (U8*)(&(b.data));
    for (int i=0; i<12; i++) {
        if (pieces[i] == p1) {
            pieces[i] = p0;
            break;
        }
    }
    if (b.data.last_killed_piece_idx >= 0) {
        pieces[b.data.last_killed_piece_idx] = p1;
        b.data.last_killed_piece_idx = -1;
    }

    if (promo == PAWN_ROOK) {
        piecetype = ((piecetype & (WHITE | BLACK)) ^ ROOK) | PAWN;
    }
    else if (promo == PAWN_BISHOP) {
        piecetype = ((piecetype & (WHITE | BLACK)) ^ BISHOP) | PAWN;
    }

    b.data.board_0[p0]           = piecetype;
    b.data.board_90[cw_90[p0]]   = piecetype;
    b.data.board_180[cw_180[p0]] = piecetype;
    b.data.board_270[acw_90[p0]] = piecetype;

    b.data.board_0[p1]           = deadpiece;
    b.data.board_90[cw_90[p1]]   = deadpiece;
    b.data.board_180[cw_180[p1]] = deadpiece;
    b.data.board_270[acw_90[p1]] = deadpiece;

    // std::cout << "Undid last move\n";
    // std::cout << all_boards_to_str(*this);
}

void undo_move(U16 move, Board &b){
    _undo_last_move(move,b);
    _flip_player(b);
}
// The recursive minimax function with alpha-beta pruning
int ply_counter = 0;
int depth_0 = 0;
int search_false = 0;
int num_moves = 0;
bool is_white = false;
int minimax(const Board &b, int depth, bool isMaximizingPlayer, int alpha, int beta,std::atomic<bool>* search) {
    Board newBoard = *(b.copy());
    int check_score = 0 ;
    if (depth == 0) {
        
        depth_0++;
        if (newBoard.in_check()) {
         check_score = (newBoard.data.player_to_play == PlayerColor::BLACK) ? 150:-150;  // If white is in check, subtract value. If black is in check, add value.
    //      std::cout << "score from evaluater at leaf" << evaluate(b) << "\n";
    //    std::cout << "score from adder at leaf" << score << "\n";
        return (is_white)? evaluate(b) + check_score - num_moves: evaluate(b) + check_score + num_moves;
     }
//        std::cout << ply_counter << " at depth 0\n"<< std::endl;
    //    std::cout << "score from evaluater at leaf" << evaluate(b) << "\n";
    //    std::cout << "score from adder at leaf" << score << "\n";
         return (is_white)? evaluate(b) - num_moves: evaluate(b) + num_moves;
    }
    if (*search == false ) {
        search_false++;
//        std::cout << search_false << " search is false\n" << std::endl;
        if (newBoard.in_check()) {
         check_score = (newBoard.data.player_to_play == PlayerColor::BLACK) ? 150:-150;  // If white is in check, subtract value. If black is in check, add value.
    //      score += check_score;
    //      std::cout << "score from evaluater at leaf" << evaluate(b) << "\n";
    //    std::cout << "score from adder at leaf" << score << "\n";
       (is_white)? evaluate(b) + check_score - num_moves: evaluate(b) + check_score + num_moves;
     }
        
    //     std::cout << "score from evaluater at leaf" << evaluate(b) << "\n";
    //    std::cout << "score from adder at leaf" << score << "\n";
          return (is_white)? evaluate(b) - num_moves: evaluate(b) + num_moves;
    }
  //  Board newBoard = *b.copy();
    
    auto moves = newBoard.get_legal_moves();
     std::vector<std::pair<int, U16>> movesorted;
   //std::vector<U16> moveset;
    //for(auto m:moves){
      //  moveset.push_back(m);
    //}
   //std::srand(time(NULL));
    //std::random_shuffle(std::begin(moveset), std::end(moveset));
    if (isMaximizingPlayer) {
       
       for (auto m:moves){
        //newBoard.do_move(m);
        movesorted.push_back(std::make_pair(-1 * (get_added_score(m,newBoard)),m));
        //undo_move(m,newBoard);
       }
       std::sort(movesorted.begin(),movesorted.end());
        int maxEval = -2000;
        for (long unsigned int i = 0;i < movesorted.size();i++) {
            //newBoard = *(b.copy());
            newBoard.do_move(movesorted[i].second);
            num_moves += 1;
            // std::cout << "score from evaluater at internal" << evaluate(b) << "\n";
            // std::cout << "score from adder at internal" << score << "\n";
            int eval = minimax(newBoard, depth - 1, false, alpha, beta,search);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            undo_move(movesorted[i].second,newBoard);
            num_moves -= 1;
            if (beta <= alpha) {
                break;  // beta cutoff
            }
        }
        if (maxEval == -2000){
             maxEval += (is_white)? -3*num_moves: 3 *num_moves;
             if (newBoard.in_check()) {
             maxEval += (newBoard.data.player_to_play == PlayerColor::BLACK) ? 150:-150;
        
        }
        else{
            maxEval += 300;
        }
        }
        return maxEval;
    } else {
         for (auto m:moves){
        //newBoard.do_move(m);
        movesorted.push_back(std::make_pair(get_added_score(m,newBoard),m));
        //undo_move(m,newBoard);
       }
       std::sort(movesorted.begin(),movesorted.end());
        int minEval = 2000;
        for (long unsigned int i = 0;i < movesorted.size();i++) {
            // newBoard = *(b.copy());
            newBoard.do_move(movesorted[i].second);
            num_moves += 1;
            int eval = minimax(newBoard, depth - 1, true, alpha, beta,search);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            undo_move(movesorted[i].second,newBoard);
            num_moves -= 1;
            if (beta <= alpha) {
                break;  // alpha cutoff
            }
        }
      if (minEval == 2000){
             minEval += (is_white)? -3*num_moves: 3 *num_moves;
             if (newBoard.in_check()) {
             minEval += (newBoard.data.player_to_play == PlayerColor::BLACK) ? 150:-150;
        
        }
        else{
            minEval -= 300;
        }
        }
        return minEval;
    }
}

void Engine::find_best_move(const Board& b) {
    auto start = std::chrono::high_resolution_clock::now();

    ply_counter = 0;
    depth_0 = 0;
    search_false = 0;

    auto moves = b.get_legal_moves();

    if (moves.size() == 0) {
        this->best_move = 0;
        return;
    }
    std::vector<U16> moveset;
    for(auto m:moves){
        moveset.push_back(m);
    }
    std::srand(time(NULL));
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(std::begin(moveset), std::end(moveset), g);
    Board newBoard = *(b.copy());
    int initial_score = evaluate(newBoard);
    while(search) {
        int bestValue;
        //num_moves = 0;
        // Minimax initialization
        is_white = false;
        if (b.data.player_to_play == WHITE) {
            is_white = true;
            bestValue = NEG_INF;
        } else { bestValue = INF; }
        U16 bestMove = 0;
        //newBoard = *(b.copy());
        std::vector<std::pair<int, U16>> movesorted;
        for (auto m: moves) {
            //newBoard.do_move(m);
            movesorted.push_back(std::make_pair(-1 *(initial_score + get_added_score(m,newBoard)), m));
            //std::cout << "score" << evaluate(newBoard) << "\n";
            //undo_move(m,newBoard);
        }
        std::sort(movesorted.begin(), movesorted.end());

        for (long unsigned int i = 0; i < movesorted.size(); i++) {
            //newBoard = *(b.copy());
            newBoard.do_move(movesorted[i].second);
            num_moves += 1;

            if (is_white) {
                int moveValue = minimax(newBoard, ply_counter, false, NEG_INF,
                                        INF,&search);  // assuming depth 3 for now, you can change as needed
                if (moveValue > bestValue) {
                    bestValue = moveValue;
                    bestMove = movesorted[i].second;
                }
            } else {
                int moveValue = minimax(newBoard, ply_counter, true, NEG_INF,
                                        INF,&search);  // assuming depth 3 for now, you can change as needed
                if (moveValue < bestValue) {
                    bestValue = moveValue;
                    bestMove = movesorted[i].second;
                }
            }
            undo_move(movesorted[i].second,newBoard);
            num_moves -= 1;
        }
        if(search_false == 0){
      
        this->best_move = bestMove;
         std::cout << "Best move: " << this->best_move  <<  "value="<< bestValue<< std::endl;
        }
       
        ply_counter++;
    }
  //  num_moves += 1;
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken by function: " << duration.count() << " ms" << std::endl;
    std::cout << "Number of ply: " << ply_counter << std::endl;
    std::cout << "Number of depth 0: " << depth_0 << std::endl;
    std::cout << "Number of search false: " << search_false << std::endl;
}

//1029 1
//1029 0
//2561 34
//2561  0
//2561  24