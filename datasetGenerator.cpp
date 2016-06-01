#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include "backgammon_utilities.h"
#include "generator.h"
#include "judge.h"

int main() {
    //сбрасываем рандомный генератор
    std::srand(std::time(NULL));

    //определяем начальную доску
    TPosition board = {15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    //и файл, в котором храним dataset
    std::ofstream out("dataset.txt");

    //Объявляем рандомные стратегии, флаг на невозможный ход,
    //устанавливаем стратегии и судью
    RandomStrategy white_strategy, black_strategy;
    std::pair<int, int> dice;
    TMove nullTMove = {std::make_pair(-1, -1)}, current;
    white_strategy.setPlayer(true, nullTMove);
    black_strategy.setPlayer(false, nullTMove);
    bool isWhite = rand() % 2 != 0;
    Judge<RandomStrategy, RandomStrategy> possibility(board, isWhite);


    int whites, blacks;
    std::vector<int> white_in_row, black_in_row;
    int i = 0;
    while(true) {
        //если на поле нет фишек какого-то цвета, начинаем заново
        if (!OnField(board, true) || !OnField(board, false))
            board = {15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        //сколько фишек выкинуто
        whites = blacks = 0;
        for (int c : board)
            if (c > 0)
                whites += c;
            else if (c < 0)
                blacks -= c;
        out << 15 - whites << ' ' << 15 - blacks;

        //выводим конфигурацию поля для каждого цвета
        for (int c : board)
            out << ' ' << ((c > 0) ? c : 0);
        for (int c : board)
            out << ' ' << ((c < 0) ? -c : 0);

        //выводим, сколько позиций, начиная с данной, заполнены фишками нашего цвета
        white_in_row = checkersInRow(board, true);
        black_in_row = checkersInRow(board, false);
        for (int c : white_in_row)
            out << ' ' << c;
        for (int c : black_in_row)
            out << ' ' << c;

        //и делаем ход, выбранный рандомной стратегией
        dice = {rand() % 6 + 1, rand() % 6 + 1};
        if (isWhite)
            current = white_strategy.makeMove(board, dice);
        else
            current = black_strategy.makeMove(board, dice);
        if (current != nullTMove)
            for (std::pair<int, int>& move : current)
                MakeStep(board, move, isWhite);
        possibility.change_board(board);
        out << ' ' << possibility.whitePossibility() << '\n';
        isWhite = !isWhite;
    }

    return 0;
}

