#include <algorithm>
#include <random>
#include <iostream>

#include "board.hpp"
#include "engine.hpp"

// Define constant for positive and negative infinity
const int INF = 1e9;
const int NEG_INF = -INF;

// Define a helper function to evaluate the board
// NOTE: This is a dummy function and should be replaced with an appropriate evaluation function for your game.
// Piece values
const int PAWN_VALUE   = 10;
const int ROOK_VALUE   = 50;
const int KING_VALUE   = 900; // This might be arbitrary since the game is over if the King is captured, but for evaluation purposes, it has a high value.
const int BISHOP_VALUE = 30;

// Piece square tables
// Note: These are placeholders. For an optimal evaluation, the values inside should be tuned based on the specifics of the game.
// Placeholder values for piece square tables.
const int PAWN_SQUARE_TABLE[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 5, 5, 5, 5, 5, 5, 5,
        1, 1, 2, 3, 3, 2, 1, 1,
        0, 0, 1, 2, 2, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, -2, -2, 1, 1, 1,
        1, 2, 2, -4, -4, 2, 2, 1,
        0, 0, 0, 0, 0, 0, 0, 0
};

const int ROOK_SQUARE_TABLE[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, 10, 10, 10, 10, 5,
        5, 10, 10, 10, 10, 10, 10, 5,
        5, 10, 10, 10, 10, 10, 10, 5,
        5, 10, 10, 10, 10, 10, 10, 5,
        5, 10, 10, 10, 10, 10, 10, 5,
        5, 10, 10, 10, 10, 10, 10, 5,
        0, 0, 0, 5, 5, 0, 0, 0
};

const int KING_SQUARE_TABLE[64] = {
        20, 30, 10, 0, 0, 10, 30, 20,
        20, 20, 0, 0, 0, 0, 20, 20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        -10, -20, -20, -20, -20, -20, -20, -10,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        0, 0, 0, 0, 0, 0, 0, 0
};

const int BISHOP_SQUARE_TABLE[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, 10, 10, 10, 10, 5,
        5, 10, 10, 10, 10, 10, 10, 5,
        5, 10, 10, 10, 10, 10, 10, 5,
        5, 10, 10, 10, 10, 10, 10, 5,
        5, 10, 10, 10, 10, 10, 10, 5,
        5, 10, 10, 10, 10, 10, 10, 5,
        0, 0, 0, 5, 5, 0, 0, 0
};

int evaluate(const Board &b) {
    int score = 0;

    // Material count
    for (int i = 0; i < 64; i++) {
        U8 piece = b.data.board_0[i];
        switch (piece & ~PlayerColor::WHITE & ~PlayerColor::BLACK) {
            case PieceType::PAWN:
                score += (piece & PlayerColor::WHITE) ? PAWN_VALUE + PAWN_SQUARE_TABLE[i] : -PAWN_VALUE - PAWN_SQUARE_TABLE[i];
                break;
            case PieceType::ROOK:
                score += (piece & PlayerColor::WHITE) ? ROOK_VALUE + ROOK_SQUARE_TABLE[i] : -ROOK_VALUE - ROOK_SQUARE_TABLE[i];
                break;
            case PieceType::KING:
                score += (piece & PlayerColor::WHITE) ? KING_VALUE + KING_SQUARE_TABLE[i] : -KING_VALUE - KING_SQUARE_TABLE[i];
                break;
            case PieceType::BISHOP:
                score += (piece & PlayerColor::WHITE) ? BISHOP_VALUE + BISHOP_SQUARE_TABLE[i] : -BISHOP_VALUE - BISHOP_SQUARE_TABLE[i];
                break;
        }
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



// The recursive minimax function with alpha-beta pruning
int Engine::minimax(const Board &b, int depth, bool isMaximizingPlayer, int alpha, int beta) {
    if (depth == 0 || search == false) {
        return evaluate(b);
    }

    auto moves = b.get_legal_moves();

    if (isMaximizingPlayer) {
        int maxEval = NEG_INF;
        for (auto m : moves) {
            Board newBoard = *b.copy();
            newBoard.do_move(m);
            int eval = minimax(newBoard, depth - 1, false, alpha, beta);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) {
                break;  // beta cutoff
            }
        }
        return maxEval;
    } else {
        int minEval = INF;
        for (auto m : moves) {
            Board newBoard = *b.copy();
            newBoard.do_move(m);
            int eval = minimax(newBoard, depth - 1, true, alpha, beta);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                break;  // alpha cutoff
            }
        }
        return minEval;
    }
}

void Engine::find_best_move(const Board& b) {
    auto moves = b.get_legal_moves();

    if (moves.size() == 0) {
        this->best_move = 0;
        return;
    }

    // Minimax initialization
    int bestValue = NEG_INF;
    U16 bestMove = 0;
    for (auto m : moves) {
        Board newBoard = *b.copy();
        newBoard.do_move(m);
        int moveValue = minimax(newBoard, 3, false, NEG_INF, INF);  // assuming depth 3 for now, you can change as needed

        if (moveValue > bestValue) {
            bestValue = moveValue;
            bestMove = m;
        }
    }

    this->best_move = bestMove;
}