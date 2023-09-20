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
const short PAWN_VALUE   = 10;
const short ROOK_VALUE   = 50;
const short KING_VALUE   = 900; // This might be arbitrary since the game is over if the King is captured, but for evaluation purposes, it has a high value.
const short BISHOP_VALUE = 30;

// Piece square tables
// Note: These are placeholders. For an optimal evaluation, the values inside should be tuned based on the specifics of the game.
// Placeholder values for piece square tables.
const short PAWN_SQUARE_TABLE[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 2, 3, 5, 5, 5, 5,
        1, 1, 2, 3, 3, 2, 1, 1,
        0, 0, 1, 2, 2, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, -2, -2, 1, 1, 1,
        1, 2, 2, -4, -4, 2, 2, 1,
        0, 0, 0, 0, 0, 0, 0, 0
};

const short ROOK_SQUARE_TABLE[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        14, 10, 10, 10, 10, 5, 5, 5,
        12, 12, 10, 10, 10, 5, 5, 5,
        11, 11, 10, 10, 10, 5, 5, 5,
        10, 10, 10, 10, 10, 5, 5, 5,
        10, 9, 7, 6, 5, 6, 3, 5,
        8, 7, 6, 5, 4, 5, 5, 5,
        0, 0, 0, 5, 5, 0, 0, 0
};

const short KING_SQUARE_TABLE[64] = {
        20, 30, 10, 0, 0, 10, 30, 20,
        20, 20, 0, 0, 0, 0, 20, 20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        -10, -20, -20, -20, -20, -20, -20, -10,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -10, -10, -10, -50, -40, -40, -30,
        -10, -10, -5, -5, -50, -40, -40, -30,
        0, 0, 0, 0, 0, 0, 0, 0
};

const short BISHOP_SQUARE_TABLE[64] = {
        10, 10, 4, 4, 4, 6, 7, 0,
        10, 10, 10, 10, 10, 3, 4, 5,
        10, 10, 10, 10, 10, 3, 4, 5,
        10, 10, 10, 10, 10, 3, 4, 5,
        10, 10, 10, 10, 10, 3, 4, 5,
        10, 10, 7, 6, 5, 4, 3, 4,
        10, 10, 10, 10, 10, 3, 4, 5,
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
                score += (piece & PlayerColor::WHITE) ? PAWN_VALUE + PAWN_SQUARE_TABLE[8*i + j] : -PAWN_VALUE - PAWN_SQUARE_TABLE[8*i + j];
                break;
            case PieceType::ROOK:
                score += (piece & PlayerColor::WHITE) ? ROOK_VALUE + ROOK_SQUARE_TABLE[8*i + j] : -ROOK_VALUE - ROOK_SQUARE_TABLE[8*i +j];
                break;
            case PieceType::KING:
                score += (piece & PlayerColor::WHITE) ? KING_VALUE + KING_SQUARE_TABLE[8*i + j] : -KING_VALUE - KING_SQUARE_TABLE[8*i + j];
                break;
            case PieceType::BISHOP:
                score += (piece & PlayerColor::WHITE) ? BISHOP_VALUE + BISHOP_SQUARE_TABLE[8*i + j] : -BISHOP_VALUE - BISHOP_SQUARE_TABLE[8*i +j];
                break;
        }}
    }

    // Check and Checkmate
    if (b.in_check()) {
        score += (b.data.player_to_play == PlayerColor::WHITE) ? -100 : 100;  // If white is in check, subtract value. If black is in check, add value.
    }
    // Note: Check for checkmate can be added as well but you haven't provided a function to determine checkmate.
    // For instance:
    // if (b.is_checkmate()) {
    //     score += (b.data.player_to_play == PlayerColor::WHITE) ? -1000 : 1000;
    // }

    return score;
}


int get_added_score(U16 move,const Board &b){
    U8 p0 = getp0(move);
    U8 p1 = getp1(move);
    U8 promo = getpromo(move);
    U8 piecetype = b.data.board_0[p0];
    U8 deadpiece = b.data.board_0[p1];
    int added = 0;
    switch (piecetype & ~PlayerColor::WHITE & ~PlayerColor::BLACK) {
            case PieceType::PAWN:
                added += (piecetype & PlayerColor::WHITE) ? PAWN_SQUARE_TABLE[p1] -  PAWN_SQUARE_TABLE[p0] : PAWN_SQUARE_TABLE[p0] - PAWN_SQUARE_TABLE[p1];
                if (promo == PAWN_ROOK){
                     added += (piecetype & PlayerColor::WHITE) ? ROOK_VALUE + ROOK_SQUARE_TABLE[p1]- PAWN_VALUE : -ROOK_VALUE - ROOK_SQUARE_TABLE[p1]+PAWN_VALUE;
                     break;
                } 
                if(promo == PAWN_BISHOP){
                    added += (piecetype & PlayerColor::WHITE) ? BISHOP_VALUE + BISHOP_SQUARE_TABLE[p1]- PAWN_VALUE:-BISHOP_VALUE - BISHOP_SQUARE_TABLE[p1]+PAWN_VALUE;
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
                added += (piecetype & PlayerColor::WHITE) ?  BISHOP_SQUARE_TABLE[p1] - BISHOP_SQUARE_TABLE[p0]: BISHOP_SQUARE_TABLE[p0] - BISHOP_SQUARE_TABLE[p1];
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
 if (b.in_check()) {
        added += (b.data.player_to_play == PlayerColor::WHITE) ? -150 : 150;  // If white is in check, subtract value. If black is in check, add value.
    }

return added;
}
// void _flip_player(const Board &b) {
//     b.data.player_to_play = (PlayerColor)(b.data.player_to_play ^ (WHITE | BLACK));
// }

// void _undo_last_move(U16 move,const Board &b) {

//     U8 p0 = getp0(move);
//     U8 p1 = getp1(move);
//     U8 promo = getpromo(move);

//     U8 piecetype = b.data.board_0[p1];
//     U8 deadpiece = b.data.last_killed_piece;
//     b.data.last_killed_piece = 0;

//     // scan and get piece from coord
//     U8 *pieces = (U8*)(&(b.data));
//     for (int i=0; i<12; i++) {
//         if (pieces[i] == p1) {
//             pieces[i] = p0;
//             break;
//         }
//     }
//     if (b.data.last_killed_piece_idx >= 0) {
//         pieces[b.data.last_killed_piece_idx] = p1;
//         b.data.last_killed_piece_idx = -1;
//     }

//     if (promo == PAWN_ROOK) {
//         piecetype = ((piecetype & (WHITE | BLACK)) ^ ROOK) | PAWN;
//     }
//     else if (promo == PAWN_BISHOP) {
//         piecetype = ((piecetype & (WHITE | BLACK)) ^ BISHOP) | PAWN;
//     }

//     b.data.board_0[p0]           = piecetype;
//     b.data.board_90[cw_90[p0]]   = piecetype;
//     b.data.board_180[cw_180[p0]] = piecetype;
//     b.data.board_270[acw_90[p0]] = piecetype;

//     b.data.board_0[p1]           = deadpiece;
//     b.data.board_90[cw_90[p1]]   = deadpiece;
//     b.data.board_180[cw_180[p1]] = deadpiece;
//     b.data.board_270[acw_90[p1]] = deadpiece;

//     // std::cout << "Undid last move\n";
//     // std::cout << all_boards_to_str(*this);
// }

// void undo_move(U16 move,const Board &b){
//     _undo_last_move(move,b);
//     _flip_player(b);
// }
// The recursive minimax function with alpha-beta pruning
int ply_counter = 0;
int depth_0 = 0;
int search_false = 0;
int Engine::minimax(const Board &b, int depth, bool isMaximizingPlayer, int alpha, int beta,int score) {
    if (depth == 0) {
        depth_0++;
//        std::cout << ply_counter << " at depth 0\n"<< std::endl;
        return score;
    }
    if (search == false ) {
        search_false++;
//        std::cout << search_false << " search is false\n" << std::endl;
         return score;
    }
  //  Board newBoard = *b.copy();
    Board newBoard = *(b.copy());
    auto moves = b.get_legal_moves();
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
        movesorted.push_back(std::make_pair(-1 * (score + get_added_score(m,newBoard)),m));
        //undo_move(m,newBoard);
       }
       std::sort(movesorted.begin(),movesorted.end());
        int maxEval = -800;
        for (long unsigned int i = 0;i < movesorted.size();i++) {
            newBoard = *(b.copy());
            newBoard.do_move(movesorted[i].second);
            int eval = minimax(newBoard, depth - 1, false, alpha, beta,-1*(movesorted[i].first));
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
          // undo_move(movesorted[i].second,newBoard);
            if (beta <= alpha) {
                break;  // beta cutoff
            }
        }
        return maxEval;
    } else {
         for (auto m:moves){
        //newBoard.do_move(m);
        movesorted.push_back(std::make_pair(score + get_added_score(m,newBoard),m));
        //undo_move(m,newBoard);
       }
       std::sort(movesorted.begin(),movesorted.end());
        int minEval = 800;
        for (long unsigned int i = 0;i < movesorted.size();i++) {
             newBoard = *(b.copy());
            newBoard.do_move(movesorted[i].second);
            int eval = minimax(newBoard, depth - 1, true, alpha, beta,movesorted[i].first);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
          // undo_move(movesorted[i].second,newBoard);
            if (beta <= alpha) {
                break;  // alpha cutoff
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
        // Minimax initialization
        bool is_white = false;
        if (b.data.player_to_play == WHITE) {
            is_white = true;
            bestValue = NEG_INF;
        } else { bestValue = INF; }
        U16 bestMove = 0;
        newBoard = *(b.copy());
        std::vector<std::pair<int, U16>> movesorted;
        for (auto m: moves) {
            //newBoard.do_move(m);
            movesorted.push_back(std::make_pair(-1 * (initial_score + get_added_score(m,newBoard)), m));
            //undo_move(m,newBoard);
        }
        std::sort(movesorted.begin(), movesorted.end());

        for (long unsigned int i = 0; i < movesorted.size(); i++) {
            newBoard = *(b.copy());
            newBoard.do_move(movesorted[i].second);

            if (is_white) {
                int moveValue = minimax(newBoard, ply_counter, false, NEG_INF,
                                        INF,-1*(movesorted[i].first));  // assuming depth 3 for now, you can change as needed
                if (moveValue > bestValue) {
                    bestValue = moveValue;
                    bestMove = movesorted[i].second;
                }
            } else {
                int moveValue = minimax(newBoard, ply_counter, true, NEG_INF,
                                        INF,-1*(movesorted[i].first));  // assuming depth 3 for now, you can change as needed
                if (moveValue < bestValue) {
                    bestValue = moveValue;
                    bestMove = movesorted[i].second;
                }
            }
            //undo_move(movesorted[i].second,newBoard);
        }

        this->best_move = bestMove;
        std::cout << "Best move: " << this->best_move << std::endl;
        ply_counter++;
    }
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken by function: " << duration.count() << " ms" << std::endl;
    std::cout << "Number of ply: " << ply_counter << std::endl;
    std::cout << "Number of depth 0: " << depth_0 << std::endl;
    std::cout << "Number of search false: " << search_false << std::endl;
}