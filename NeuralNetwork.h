//
// Created by Pavel on 23.05.2016.
//

#ifndef BACKGAMMON_NEURALNETWORK_H
#define BACKGAMMON_NEURALNETWORK_H

#include <vector>
#include <cmath>
#include "backgammon_utilities.h"

class NeuralNetwork : public IStrategy {
    std::vector<std::vector<std::vector<double>>> matrixes;
    std::vector<std::vector<double>> intercepts;
    bool isWhite;

    double sigmoid(double x) {
        return 1.0/(1.0 + exp(-x));
    }

    std::vector<double> multiply(std::vector<std::vector<double>> matrix, std::vector<double> column, std::vector<double> intercept) {
        std::vector<double> answer(matrix.size());
        for (size_t i = 0; i < matrix.size(); ++i) {
            for (size_t j = 0; j < column.size(); ++j) {
                answer[i] += matrix[i][j]*column[j];
            }
            answer[i] = sigmoid(answer[i] + intercept[i]);
        }
        return answer;
    }

    double calculatePossibility(TMove generated, TPosition& board) {
        for (std::pair<int, int>& move : generated)
            MakeStep(board, move, isWhite);

        std::vector<double> config = generateConfiguration(board);

        //считаем вероятность
        for (size_t i = 0; i < matrixes.size(); ++i) {
            config = multiply(matrixes[i], config, intercepts[i]);
        }

        for (std::pair<int, int>& move : generated)
            MakeStep(board, move, !isWhite);

        return config[0];
    }

public:

    NeuralNetwork(const std::vector<int> layers) {
        matrixes.resize(layers.size() - 1);
        for (size_t i = 0; i < layers.size() - 1; ++i) {
            matrixes[i].resize(layers[i + 1]);
            for (size_t j = 0; j < layers[i + 1]; ++j)
                matrixes[i][j].resize(layers[i]);
        }
        std::cout << "OK";
        intercepts.resize(layers.size() - 1);
        for (size_t i = 0; i < layers.size() - 1; ++i) {
            intercepts[i].resize(layers[i + 1]);
        }
        std::cout << "OKK";
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
        double probability = -1000;
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

    void read_matrix(std::istream& cin) {
        for (size_t i = 0; i < matrixes.size(); ++i) {
            for (size_t j = 0; j < matrixes[i].size(); ++j) {
                for (size_t k = 0; k < matrixes[i][j].size(); ++k) {
                    cin >> matrixes[i][j][k];
                }
            }
        }
        for (size_t i = 0; i < intercepts.size(); ++i) {
            for (size_t j = 0; j < intercepts[i].size(); ++j) {
                cin >> intercepts[i][j];
            }
        }
    }
};

#endif //BACKGAMMON_NEURALNETWORK_H
