//
// Created by Pavel on 17.04.2016.
//

#ifndef BACKGAMMON_JUDGE_H
#define BACKGAMMON_JUDGE_H

#include <algorithm>
#include <iostream>
#include <vector>
#include "backgammon_utilities.h"
#include "generator.h"

class Judge {
    TPosition start_board;
    bool isWhite;
    IStrategy* white_player;
    IStrategy* black_player;
    TMove nullTMove;
public:
    Judge(IStrategy* white, IStrategy* black) {
        start_board = {15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        isWhite = true;
        //флаг, определяющий, что ход сделать нельзя
        nullTMove = {std::make_pair(-1, -1)};
        //устанавливаем стратегии
        white_player = white;
        black_player = black;
        white_player->setPlayer(true, nullTMove);
        black_player->setPlayer(false, nullTMove);
    }
    Judge(IStrategy* white, IStrategy* black, TPosition& start, bool color) {
        start_board = start;
        isWhite = color;
        nullTMove = {std::make_pair(-1, -1)};
        white_player = white;
        black_player = black;
        white_player->setPlayer(true, nullTMove);
        black_player->setPlayer(false, nullTMove);
    }
    double whitePossibility() {
        TMove generated;
        //количество побед белых
        int white_win = 0;
        for (size_t i = 0; i < 100; ++i) {
            TPosition board = start_board;
            isWhite = rand() % 2 != 0;
            //играем, пока на поле есть фишки обоих цветов
            while (OnField(board, true) && OnField(board, false)) {
                std::pair<int, int> dice = {rand() % 6 + 1, rand() % 6 + 1};
                std::vector<TMove> moves = PossibleMoves(board, dice, isWhite);
                if (isWhite)
                    generated = white_player->GetMove(board, moves);
                else
                    generated = black_player->GetMove(board, moves);
                //если есть ход, делаем его
                if (generated != nullTMove)
                    for (std::pair<int, int> &move : generated)
                        MakeStep(board, move, isWhite);
                //и передаем ход противнику
                isWhite = !isWhite;
            }
            if (!OnField(board, true))
                white_win++;
        }
        return double(white_win) / 100;
    }
    void change_board(TPosition& board) {
        start_board = board;
    }
};
#endif //BACKGAMMON_JUDGE_H
