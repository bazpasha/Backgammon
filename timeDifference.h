//
// Created by Pavel on 17.04.2016.
//

#ifndef BACKGAMMON_TIMEDIFFERENCE_H
#define BACKGAMMON_TIMEDIFFERENCE_H

#include <iostream>
#include <vector>
#include "backgammon_utilities.h"

class TimeDifference : public IStrategy {
    std::vector<double> coef;
    bool isWhite;
    double calculatePossibility(TMove generated, TPosition& board) {
        for (std::pair<int, int>& move : generated)
            MakeStep(board, move, isWhite);

        std::vector<double> config = generateConfiguration(board);

        //считаем вероятность
        double probability = 0;
        for (int i = 0; i < coef.size(); ++i)
            probability += coef[i] * config[i];

        for (std::pair<int, int>& move : generated)
            MakeStep(board, move, !isWhite);

        return probability;
    }

public:
    TimeDifference() {
        coef = {0.00139877, 0.000591867, 0.000569581, 0.000149646, -5.61636e-005, 0.000109346, 0.000204908, 9.2077e-005,
                0.000224193, -0.000139011, 0.00020969, 0.000250672, -0.000129035, 0.000144201, 4.91608e-005,
                -0.000252112, 0.000136132, -5.43466e-006, 0.000172093, -3.23806e-005, 0.000111159, -9.26365e-005,
                0.000289133, 0.000303426, 0.000183807, 0.00037006, -0.000219362, 0.000314668, 6.41228e-005, 0.000329058,
                0.00013634, 0.000263966, 0.000134333, 0.000279126, 0.000694034, -0.000116896, 9.05993e-005, 0.00103002,
                9.24826e-005, 0.000179665, 0.00013874, 0.000262346, 1.36822e-005, -0.000265071, 0.000128783,
                0.000355409, -4.40115e-005, -8.39971e-006, -9.95784e-005, -8.46441e-005, -0.000147877, 1.37121e-005,
                -5.61636e-005, -5.5046e-005, -4.75315e-005, -7.94221e-006, 0.000221644, 2.31493e-005, -0.000135515,
                7.14791e-005, -0.000343754, 0.00018663, 0.000139549, 0.00016398, -0.000241643, -0.000364111,
                8.80521e-005, -0.000287208, 9.99327e-005, 0.000364262, 0.000557165, 0.000642124, 6.44554e-005,
                -0.000265278, 0.000212627, 0.000304801, 7.07592e-005, 3.09879e-005, -0.00017564, -7.47374e-005,
                -0.000160722, 0.000466114, -0.000240429, 0.000156143, 9.46958e-005, -0.000129033, -0.000129782,
                -6.32126e-005, 0.000178794, 0.000224928, -5.58324e-005, -0.000179025, 2.91153e-005, 8.06745e-005,
                -3.396e-005, -0.000180223, 0.000338514, -0.000149521};
    }
    virtual TMove GetMove(const std::vector<int>& board, const std::vector<TMove>& possibleMoves) {
        isWhite = true;
        if (possibleMoves.size())
            for (auto first_move : possibleMoves[0]) {
                if (first_move.first > 0) {
                    isWhite = true;
                    break;
                } else if (first_move.first > 0) {
                    isWhite = false;
                    break;
                }
            }
        TPosition copy_board = board;
        TMove answer = {};
        double probability = -10;
        for (TMove move : possibleMoves)
            if (calculatePossibility(move, copy_board) > probability && isWhite) {
                probability = calculatePossibility(move, copy_board);
                answer = move;
            } else if (1 - calculatePossibility(move, copy_board) > probability && !isWhite) {
                probability = 1 - calculatePossibility(move, copy_board);
                answer = move;
            }
        return answer;
    }
};

#endif //BACKGAMMON_TIMEDIFFERENCE_H
