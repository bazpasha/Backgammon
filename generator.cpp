#include <algorithm>
#include <iostream>
#include <vector>
#include <set>

using TMove = std::vector<std::pair<int, int>>;
using TPosition = std::vector<int>;

bool InitialPosition(TPosition& board) {
    /*
     * Возвращает true, если ни одного хода еще не было сделано
     */

    return (board[0] == 15 && board[12] == -15);
}

bool IllegalBlockingPosition(TPosition& board) {
    /*
     * Возвращает true, если образовался блок из 6 фишек,
     * при этом противник еще не переступал через наши фишки
     */

    // сколько черных фишек стоит после белых
    int black_after_white = 0;
    for (int i = 23; board[i] <= 0 && i >= 0; --i)
        black_after_white += board[i];

    // если стоят не все, то можно не беспокоиться
    if (black_after_white < 15)
        return false;

    // иначе проверяем на группу из 6 фишек
    bool sixInRow;
    for (size_t i = 0; i < 6; ++i) {
        sixInRow = true;
        for (size_t j = i; j < i + 6; ++j)
            if (board[j] <= 0)
                sixInRow = false;
        if (sixInRow)
            return true;
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

bool OnField(TPosition& board) {
    /*
     * Возврашает true, если на поле остались фишки нужного цвета
     */

    for (size_t i = 0; i < 24; ++i)
        if (board[i] > 0)
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
            for (j = 23; j >= 18; --j)
                if (board[j] > 0)
                    break;
            if (j < 24 - die && j > 17) {
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
    int die;

    // осталось только одно значение
    if (dice.size() == 1)
        return SingleStep(board, dice[0], AtHome(board), start);

    // есть еще несколько значений
    else {
        for (size_t i = 0; i < dice.size(); ++i) {
            die = dice[0];
            dice.erase(dice.begin(), dice.begin() + 1);

            // генерируем возможные ходы с данным значением кубика
            possible_moves = SingleStep(board, die, AtHome(board), start);

            for (size_t j = 0; j < possible_moves.size(); ++j) {
                MakeStep(board, possible_moves[j][0], true);

                // генерирует ходы с оставшимися значениями кубиков; с головы берем только один раз
                if (possible_moves[j][0].first)
                    additional_moves = Generate(board, dice, possible_moves[j][0].first);
                else
                    additional_moves = Generate(board, dice, 1);

                // добавляет текущий ход к сгенерированным
                for (auto &additional_move : additional_moves) {
                    additional_move.insert(additional_move.begin(), possible_moves[j][0]);
                    moves.insert(moves.begin(), additional_move);
                }

                // а если на поле не осталось фишек или ходов дальше нет, просто добавляем текущий
                if (!OnField(board) || additional_moves.size() == 0)
                    moves.insert(moves.begin(), possible_moves[j]);

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

    std::vector<TMove> generated_moves;
    std::vector<TMove> result;

    // если играем черными - трансформируем доску...
    if (!isWhite)
        BlackBoard(board);

    // рассматриваем отдельные случаи первого хода
    if (InitialPosition(board) && dice.first == 3 && dice.second == 3) {
        TMove move1 = {std::make_pair(0, 3), std::make_pair(0, 3), std::make_pair(3, 6), std::make_pair(3, 6)};
        TMove move2 = {std::make_pair(0, 3), std::make_pair(0, 3), std::make_pair(3, 6), std::make_pair(6, 9)};
        result = {move1, move2};
        return result;
    }
    else if (InitialPosition(board) && dice.first == 4 && dice.second == 4) {
        TMove move = {std::make_pair(0, 4), std::make_pair(0, 4), std::make_pair(4, 8), std::make_pair(4, 8)};
        result = {move};
        return result;
    }
    else if (InitialPosition(board) && dice.first == 6 && dice.second == 6) {
        TMove move = {std::make_pair(0, 6), std::make_pair(0, 6)};
        result = {move};
        return result;
    }

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

    // находим максимально длинные ходы
    size_t mx_size = 0;
    for (TMove& move : generated_moves) {
        if (mx_size < move.size())
            mx_size = move.size();
        // трансформируем ходы, в случае если играем черными
        if (!isWhite) {
            for (auto& step : move) {
                step.first = BlackNotation(step.first);
                step.second = BlackNotation(step.second);
            }
        }
    }

    // и убираем повторяющиеся
    std::set<TMove> answer_set;
    for (TMove &move : generated_moves) {
        if (move.size() == mx_size) {
            std::sort(move.begin(), move.end());
            answer_set.insert(move);
        }
    }

    for (TMove move : answer_set)
        result.push_back(move);

    // ...и обратно
    if (!isWhite)
        BlackBoard(board);

    return result;
}

int main() {
    TPosition board = {15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::pair<int, int> dice = std::make_pair(6, 6);
    std::vector<TMove> result = PossibleMoves(board, dice, false);

    for (TMove move : result){
        for (auto step : move)
            std::cout << step.first << " -> " << step.second << "; ";
        std::cout << std::endl;
    }
    return 0;
}