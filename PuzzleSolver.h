#ifndef PUZZLE_PUZZLESOLVER_H
#define PUZZLE_PUZZLESOLVER_H


#include <vector>
#include <memory>
#include <fstream>
#include <deque>
#include <climits>
#include "AbstractPieceManager.h"
#include "PuzzlePieceConstrain.h"

using namespace std;

class PuzzleSolver {
private:
    struct PuzzleLocation {
        int row, col;
    };

    struct PuzzlePieceData {
        PuzzleLocation location;
        Piece_t current;
    };

    vector<vector<Piece_t>> puzzleSolution;
    vector<vector<Piece_t>> puzzleConstrain;

    void createNewPuzzleSolution(AbstractPieceManager::Shape shape);

    const unique_ptr<AbstractPieceManager> &pieceManager;

    bool trySolveForShape(AbstractPieceManager::Shape shape);

    bool tryGettingNextPuzzleLocationToFill(PuzzleLocation &bestLocation);

    void updatePieceInSolution(PuzzleLocation puzzleLocation, Piece_t currentPiece);

    void removePieceFromSolution(PuzzleLocation puzzleLocation);

    inline Piece_t getConstrainOpposite(Piece_t currentConstrain);

public:

    explicit PuzzleSolver(const unique_ptr<AbstractPieceManager> &pieceManager);

    bool trySolve();

    void exportSolution(ofstream &out);

    void exportErrors(ofstream &out);
};


#endif //PUZZLE_PUZZLESOLVER_H