//
// Created by t-idkess on 11/11/2017.
//

#ifndef PAZZEL_PUZZLESOLVER_H
#define PAZZEL_PUZZLESOLVER_H

#include "PieceManager.h"
#include "PuzzlePieceConstrain.h"
#include <vector>

class SolverErrors {
public:
    bool wrongNumberOfStraightLine = false;
    bool missingTL = false, missingTR = false, missingBL = false, missingBR = false;
    bool sumOfEdgesIsNotZero = false;
    bool couldNotSolvePuzzle = false;

    bool hasError();
};

class PuzzleSolution {
    Piece_t *puzzleSolution;
public:
    int row, col;

    PuzzleSolution(int row, int col);

    ~PuzzleSolution();

    Piece_t get(int _row, int _col);

    void set(int _row, int _col, Piece_t newVal);
};

class PuzzleSolver {
    PieceManager pieceManager;

    bool solvePuzzle(int row, int col);

public:
    SolverErrors solverErrors;
    PuzzleSolution *puzzleSolution = nullptr;

    explicit PuzzleSolver(ParsedPuzzle &puzzle);

    ~PuzzleSolver();
};


#endif //PAZZEL_PUZZLESOLVER_H
