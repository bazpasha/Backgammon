//
// Created by Pavel on 17.04.2016.
//

#ifndef BACKGAMMON_LINEARREGRESSION_H
#define BACKGAMMON_LINEARREGRESSION_H

#include <iostream>
#include <vector>
#include "backgammon_utilities.h"

class LinearRegression : public IStrategy {
    std::vector<double> coef;
    double intercept;
    bool isWhite;
    double calculatePossibility(TMove generated, TPosition& board) {
        for (std::pair<int, int>& move : generated)
            MakeStep(board, move, isWhite);

        std::vector<double> config = generateConfiguration(board);

        //считаем вероятность
        double probability = intercept;
        for (int i = 0; i < coef.size(); ++i)
            probability += coef[i] * config[i];

        for (std::pair<int, int>& move : generated)
            MakeStep(board, move, !isWhite);

        return probability;
    }

 public:
    LinearRegression() {
        intercept = 0.490836803298;
        coef = {-2058729328.83, 2058729328.85, -2058729331.99, -2058729331.82, -2058729331.76, -2058729331.61,
                -2058729331.61, -2058729331.67, -2058729331.47, -2058729331.19, -2058729331.01, -2058729330.89,
                -2058729330.77, -2058729330.69, -2058729330.59, -2058729330.45, -2058729330.34, -2058729330.25,
                -2058729330.16, -2058729330.07, -2058729329.92, -2058729329.91, -2058729329.83, -2058729329.73,
                -2058729329.68, -2058729329.67, 2058729330.52, 2058729330.52, 2058729330.39, 2058729330.23,
                2058729330.18, 2058729330.08, 2058729329.98, 2058729329.85, 2058729329.8, 2058729329.7,
                2058729329.7, 2058729329.68, 2058729332.0, 2058729331.85, 2058729331.69, 2058729331.62,
                2058729331.63, 2058729331.67, 2058729331.5, 2058729331.15, 2058729330.99, 2058729330.88,
                2058729330.83, 2058729330.67, 0.167688846588, -0.0943224430084, 0.0886385440826, -0.0154345035553,
                0.0561048984528, 0.0959866046906, 0.0619026422501, 0.00989699363708, 0.0172002911568, 0.0257347822189,
                0.0147964954376, 0.0893338918686, -0.0615401268005, 0.00779038667679, 0.0110065937042, 0.00886166095734,
                -0.0316418409348, -0.0295385122299, 0.0109946727753, 0.0949082970619, 0.078798353672, 0.0741140246391,
                0.0746484994888, -0.105007648468, -0.188425064087, 0.114948451519, -0.0285786986351, -0.0179439783096,
                -0.0768805742264, -0.0663443803787, -0.0796604156494, 0.029622733593, -0.011830329895, -0.031796336174,
                -0.076672911644, 0.00539612770081, 0.0916283726692, -0.0753687620163, -0.0209527015686,
                -0.00267112255096, -0.0559010505676, 0.0778343677521, -0.0260616242886, -0.0272649526596,
                -0.0830295085907, -0.0928187966347, -0.0968210697174, 0.150573253632};
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

#endif //BACKGAMMON_LINEARREGRESSION_H
