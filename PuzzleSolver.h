#ifndef PUZZLE_PUZZLESOLVER_H
#define PUZZLE_PUZZLESOLVER_H

#include "AbstractPieceManager.h"
#include "PuzzlePieceConstrain.h"
#include <vector>
#include <memory>
#include <fstream>
#include <deque>
#include "limits.h"

using namespace std;

class SolverErrors {
public:
    bool wrongNumberOfStraightLine = false;
    bool missingTL = false, missingTR = false, missingBL = false, missingBR = false;
    bool sumOfEdgesIsNotZero = false;
    bool couldNotSolvePuzzle = false;

    bool hasError();
};


class PuzzleSolver {
private:

    struct PuzzleLocation {
        int row, col;
    };

    vector<vector<Piece_t>> puzzleSolution;
    vector<vector<Piece_t>> puzzleConstrain;

    void createNewPuzzleSolution(AbstractPieceManager::Shape shape);

    const unique_ptr<AbstractPieceManager>& pieceManager;

    bool trySolveForShape(AbstractPieceManager::Shape shape);

    PuzzleLocation getNextPuzzleLocationToFill();

    void updatePieceInSolution(PuzzleLocation puzzleLocation, Piece_t currentPiece);

    void removePieceFromSolution(PuzzleLocation puzzleLocation);

    inline Piece_t getConstrainOpposite(Piece_t currentConstrain);

public:

    explicit PuzzleSolver(const unique_ptr<AbstractPieceManager>& pieceManager);

    bool trySolve();

    void exportSolution(ofstream &out);

    void exportErrors(ofstream &out);
};


#endif //PUZZLE_PUZZLESOLVER_H