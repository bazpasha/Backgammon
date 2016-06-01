//
// Created by Pavel on 18.03.2016.
//

#ifndef BACKGAMMON_BACKGAMMON_UTILITIES_H
#define BACKGAMMON_BACKGAMMON_UTILITIES_H

#include <iostream>
#include <vector>
#include <random>

using TMove = std::vector<std::pair<int, int>>;
using TPosition = std::vector<int>;

bool InitialPosition(TPosition& board) {
    /*
     * Возвращает true, если ни одного хода еще не было сделано
     */

    return (board[0] == 15 || board[12] == -15);
}

bool IllegalBlockingPosition(TPosition& board) {
    /*
     * Возвращает true, если образовался блок из 6 фишек,
     * при этом противник еще не переступал через наши фишки
     */

    // иначе проверяем на группу из 6 фишек
    bool sixInRow;
    int black = 0;
    for (size_t i = 0; i <= 6; ++i) {
        sixInRow = true;
        for (size_t j = i; j < i + 6; ++j)
            if (board[j] <= 0)
                sixInRow = false;
        if (sixInRow) {
            black = 0;
            for (size_t j = 12; j < 24; ++j)
                if (board[j] < 0)
                    black -= board[j];
            for (size_t j = 0; j < i; ++j)
                if (board[j] < 0)
                    black -= board[j];
            if (black == 15)
                return true;
        }
    }

    return false;
}

void BlackBoard(TPosition& board) {
    /*
     * Трансформирует доску для черных
     */

    for (size_t i = 12; i < 24; ++i) {
        std::swap(board[i], board[i - 12]);
        board[i] *= -1;
        board[i - 12] *= -1;
    }
}

int BlackNotation(int pos) {
    /*
     * Трансформируем позицию для черных
     */

    if (pos == -1)
        return -1;
    else if (pos < 12)
        return pos + 12;
    else
        return pos - 12;
}

bool AtHome(TPosition& board) {
    /*
     * Возвращает true, если фишки заведены в дом
     */

    for (size_t i = 0; i < 18; ++i)
        if (board[i] > 0)
            return false;
    return true;
}

bool OnField(TPosition& board, bool isWhite) {
    /*
     * Возврашает true, если на поле остались фишки нужного цвета
     */

    int mult = isWhite ? 1 : -1;
    for (size_t i = 0; i < 24; ++i)
        if (board[i] * mult > 0)
            return true;
    return false;
}

void MakeStep(TPosition& board, std::pair<int, int> step, bool forward) {
    /*
     * По паре (from, to) делает ход на доске
     * forward = true означает ход вперед
     * иначе - откат
     */

    int inc = (forward) ? -1 : 1;
    board[step.first] += inc;
    if (step.second != -1)
        board[step.second] -= inc;
}

class IStrategy {
public:
    virtual ~IStrategy(){}
    virtual TMove GetMove(const std::vector<int>& board, const std::vector<TMove>& possibleMoves) = 0;
};

std::vector<int> checkersInRow(TPosition& board, bool color) {
    /*
     * Сколько фишек стоит подряд, начиная с данной позиции
     */
    if (!color)
        BlackBoard(board);
    std::vector<int> answer(24);
    for (int i = 0; i < 24; ++i) {
        for (int j = i; j < 24 && board[j] > 0; ++j)
            //answer[i] += board[j];
            answer[i]++;
    }
    if (!color)
        BlackBoard(board);
    return answer;
}

void normalize(std::vector<double>& config) {
    int summ = 0;
    for (double c : config)
        summ += c*c;
    double module = sqrt(summ + .0);
    for (double& c : config)
        c /= module;
}

int len(TMove move) {
    int num = 0;
    for (auto p : move)
        num += (p.second - p.first > 0 ? p.second - p.first : 0);
    return num;
}

std::vector<double> generateConfiguration(TPosition& board) {
    std::vector<double> config;
    int whites, blacks;
    //сколько фишек выкинуто
    whites = blacks = 0;
    for (int c : board)
        if (c > 0)
            whites += c;
        else if (c < 0)
            blacks -= c;
    config.push_back(15 - whites);
    config.push_back(15 - blacks);

    //выводим конфигурацию поля для каждого цвета
    for (int c : board)
        config.push_back((c > 0) ? c : 0);
    for (int c : board)
        config.push_back((c < 0) ? -c : 0);

    //выводим, сколько позиций, начиная с данной, заполнены фишками нашего цвета
    std::vector<int> white_in_row, black_in_row;
    white_in_row = checkersInRow(board, true);
    black_in_row = checkersInRow(board, false);
    for (int c : white_in_row)
        config.push_back(c);
    for (int c : black_in_row)
        config.push_back(c);
    normalize(config);
    return config;
}

#endif //BACKGAMMON_BACKGAMMON_UTILITIES_H
