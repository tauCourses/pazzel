#ifndef PUZZLE_PUZZLESOLVER_H
#define PUZZLE_PUZZLESOLVER_H

#include "AbstractPieceManager.h"
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
    AbstractPieceManager pieceManager;
    SolverErrors solverErrors;
    PuzzleSolution *puzzleSolution = nullptr;

    bool solvePuzzle(int row, int col);
    vector<int,int> getAllPuzzleShapes();

public:

    explicit PuzzleSolver(AbstractPieceManager &pieceManager);

    ~PuzzleSolver();
    bool trySolve();
    void exportErrors(char* outputFile);
};


#endif //PUZZLE_PUZZLESOLVER_H
