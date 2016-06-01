#include <iostream>
#include <vector>
#include "linearRegression.h"
#include "backgammon_utilities.h"
#include "generator.h"
#include <string>
#include <stdlib.h>

class Play {
    TPosition board;
    IStrategy *strategy_;
    bool isWhite, initial;
    std::string to_str(int k) {
        std::string result = "00";
        if (k < 10)
            result[1] += k;
        else {
            result[0] += k / 10;
            result[1] += k % 10;
        }
        return result;
    }
public:
    Play(IStrategy *strategy) {
        strategy_ = strategy;
        board = {15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        isWhite = rand() % 2 == 0;
        initial = true;
    }

    void printBoard() {
        for (int i = 11; i >= 0; --i)
            std::cout << to_str(i) << " ";
        std::cout << '\n';
        for (size_t i = 0; i < 12; ++i)
            std::cout << " - ";
        std::cout << '\n';
        for (int i = 1; i <= 15; ++i) {
            for (size_t j = 12; j > 0; --j) {
                if (board[j - 1] <= -i && board[j - 1] < 0)
                    std::cout << " x ";
                else if (board[j - 1] >= i && board[j - 1] > 0)
                    std::cout << " o ";
                else
                    std::cout << "   ";
            }
            std::cout << '\n';
        }
        for (size_t i = 0; i < 12; ++i)
            std::cout << "---";
        std::cout << '\n';
        for (int i = 15; i > 0; --i) {
            for (size_t j = 13; j <= 24; ++j) {
                if (board[j - 1] <= -i && board[j - 1] < 0)
                    std::cout << " x ";
                else if (board[j - 1] >= i && board[j - 1] > 0)
                    std::cout << " o ";
                else
                    std::cout << "   ";
            }
            std::cout << '\n';
        }
        for (size_t i = 0; i < 12; ++i)
            std::cout << " - ";
        std::cout << '\n';
        for (size_t i = 12; i < 24; ++i)
            std::cout << to_str(i) << " ";
        std::cout << '\n';
    }

    void visualize() {
        std::vector<TMove> moves;
        std::pair<int, int> dice;
        TMove strategy_move, person_move;
        while (OnField(board, true) && OnField(board, false)) {
            if (initial) {
                std::cout << "You play " << (isWhite ? "black" : "white") << "!\n";
            }

            dice = {rand() % 6 + 1, rand() % 6 + 1};
            moves = PossibleMoves(board, dice, isWhite);
            strategy_move = strategy_->GetMove(board, moves);

            std::cout << "Strategy's gotten dice " << dice.first << ' ' << dice.second << '\n';
            std::cout << "And it's made move: ";

            for (auto p : strategy_move) {
                MakeStep(board, p, isWhite);
                std::cout << "(" << p.first << "; " << p.second << ") ";
            }

            std::cout << '\n';
            
            dice = {rand() % 6 + 1, rand() % 6 + 1};
            std::cout << "\nYou've gotten dice " << dice.first << " " << dice.second << '\n';
            printBoard();
            moves = PossibleMoves(board, dice, !isWhite);
            person_move.clear();
            if (moves.size() == 0) {
                std::cout << "You don't have any possible moves :(\n";
            } else {
                person_move.resize(moves[0].size());
                std::cout << "Please, print your " << moves[0].size() << " moves in:\n";
                for (size_t i = 0; i < moves[0].size(); ++i) {
                    std::cin >> person_move[i].first >> person_move[i].second;
                }
                std::cout << "Thus, your steps are: ";
                for (auto p : person_move) {
                    MakeStep(board, p, !isWhite);
                    std::cout << "(" << p.first << "; " << p.second << ") ";
                }
                std::cout << '\n';
            }
            std::cout << "-----------------------------";
        }

        if (OnField(board, isWhite)) {
            std::cout << "Congrats! You won!";
        } else {
            std::cout << "Next time you'll be luckier";
        }
    }
};


int main() {
    LinearRegression strategy;
    Play game(&strategy);
    game.visualize();
    return 0;
}
