//
// Created by Pavel on 18.03.2016.
//

#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <set>
#include "backgammon_utilities.h"


#ifndef BACKGAMMON_GENERATOR_H
#define BACKGAMMON_GENERATOR_H

std::vector<TMove> SingleStep(TPosition& board, int die, bool atHome, int start) {
    /*
     * Возвращает список единичных ходов, которые можно
     * совершить с переданным значениям кубика
     */

    TMove move;
    std::vector<TMove> moves;
    std::pair<int, int> step;
    int to;

    for (int i = start; i < board.size() - die; ++i) {
        to = i + die;
        if (board[i] > 0 && board[to] >= 0) {
            step = std::make_pair(i, to);
            MakeStep(board, step, true);

            // не образовался блок из 6 раньше времени - все хорошо
            if (!IllegalBlockingPosition(board)) {
                move.push_back(step);
                moves.push_back(move);
                move.clear();
            }

            MakeStep(board, step, false);
        }
    }

    // если фишки в доме, то их можно выкидывать
    if (atHome) {
        // если кубик указал на позицию с нашими фишками - выкидываем
        if (board[24 - die] > 0) {
            move.push_back(std::make_pair(24 - die, -1));
            moves.push_back(move);
            move.clear();
        }

            // иначе можно выкинуть фишку с более ранней позиции,
            // если ее нет на более поздних
        else {
            size_t j;
            for (j = 18; j <= 23; ++j)
                if (board[j] > 0)
                    break;
            if (j > 24 - die && j < 24) {
                move.push_back(std::make_pair(j, -1));
                moves.push_back(move);
                move.clear();
            }
        }
    }
    return moves;
}

std::vector<TMove> Generate(TPosition& board, std::vector<int>& dice, int start) {
    /*
     * Генерирует все возможные ходы по набору значений кубиков
     */

    std::vector<TMove> moves, possible_moves, additional_moves;
    int die, max_size = 0, different_dice = 0;

    // осталось только одно значение
    if (dice.size() == 1)
        return SingleStep(board, dice[0], AtHome(board), start);

    // есть еще несколько значений
    else {
        // сколько различных значений кубиков есть?
        if (dice[0] != dice[1])
            different_dice = 2;
        else
            different_dice = 1;

        for (size_t i = 0; i < dice.size() && i < different_dice; ++i) {
            die = dice[0];
            dice.erase(dice.begin(), dice.begin() + 1);

            // генерируем возможные ходы с данным значением кубика
            possible_moves = SingleStep(board, die, AtHome(board), start);

            for (size_t j = 0; j < possible_moves.size(); ++j) {
                MakeStep(board, possible_moves[j][0], true);

                // генерирует ходы с оставшимися значениями кубиков; с головы берем только один раз
                if (possible_moves[j][0].first && start == 0)
                    additional_moves = Generate(board, dice, 0);
                else
                    additional_moves = Generate(board, dice, 1);

                // добавляет текущий ход к сгенерированным
                for (auto &additional_move : additional_moves) {
                    additional_move.push_back(possible_moves[j][0]);
                    // сохраняем только самые длинные ходы
                    if (additional_move.size() < max_size)
                        continue;
                    if (additional_move.size() > max_size) {
                        moves.clear();
                        max_size = additional_move.size();
                    }
                    moves.push_back(additional_move);
                }

                // а если на поле не осталось фишек или ходов дальше нет, просто добавляем текущий
                if ((!OnField(board, true) || additional_moves.size() == 0) && possible_moves[j].size() >= max_size) {
                    if (possible_moves[j].size() > max_size) {
                        moves.clear();
                        max_size = possible_moves[j].size();
                    }
                    moves.push_back(possible_moves[j]);
                }

                MakeStep(board, possible_moves[j][0], false);
            }

            dice.push_back(die);
        }
        return moves;
    }
}

std::vector<TMove> PossibleMoves(TPosition& board, std::pair<int, int>& dice, bool isWhite) {
    /*
     * Возвращает все возможные ходы по кубикам
     * в отсортированном порядке без повторений
     */

    std::vector<TMove> generated_moves, moves;
    std::vector<TMove> result;

    // если играем черными - трансформируем доску...
    if (!isWhite)
        BlackBoard(board);

    // рассматриваем отдельные случаи первого хода
    if (board[12] == -14 && board[8] == -1 && board[0] == 15 && dice.first == 4 && dice.second == 4) {
        TMove move = {std::make_pair(0, 4)};
        generated_moves = {move};
    }
    else if (InitialPosition(board) && dice.first == 3 && dice.second == 3) {
        TMove move1 = {std::make_pair(0, 3), std::make_pair(0, 3), std::make_pair(3, 6), std::make_pair(3, 6)};
        TMove move2 = {std::make_pair(0, 3), std::make_pair(0, 3), std::make_pair(3, 6), std::make_pair(6, 9)};
        generated_moves = {move1, move2};
    }
    else if (InitialPosition(board) && dice.first == 4 && dice.second == 4) {
        TMove move = {std::make_pair(0, 4), std::make_pair(0, 4), std::make_pair(4, 8), std::make_pair(4, 8)};
        generated_moves = {move};
    }
    else if (InitialPosition(board) && dice.first == 6 && dice.second == 6) {
        TMove move = {std::make_pair(0, 6), std::make_pair(0, 6)};
        generated_moves = {move};
    }

    else {
        // смотрим, выпал ли дубль
        if (dice.first == dice.second) {
            int die = dice.first;
            std::vector<int> double_dice = {die, die, die, die};
            generated_moves = Generate(board, double_dice, 0);
        }
            // или неравные значения
        else {
            std::vector<int> dice_array = {dice.first, dice.second};
            generated_moves = Generate(board, dice_array, 0);
        }
        if (generated_moves.size() && generated_moves[0].size() == 1) {
            int mx = 0;
            for (TMove mv : generated_moves)
                if (len(mv) > mx)
                    mx = len(mv);
            for (TMove mv : generated_moves)
                if (len(mv) == mx || mv[0].second == -1)
                    moves.push_back(mv);
            generated_moves = moves;
        }
    }

    // трансформируем ходы, в случае если играем черными
    if (!isWhite)
    for (TMove& move : generated_moves) {
        for (auto& step : move) {
            step.first = BlackNotation(step.first);
            step.second = BlackNotation(step.second);
        }
    }

    // ...и обратно
    if (!isWhite)
        BlackBoard(board);

    return generated_moves;
}

class RandomStrategy : public IStrategy {
public:
    virtual TMove GetMove(const std::vector<int>& board, const std::vector<TMove>& possibleMoves) {
        TMove move = {};
        if (possibleMoves.size() > 0)
            move = possibleMoves[rand() % possibleMoves.size()];
        return move;
    }
};
#endif //BACKGAMMON_GENERATOR_H