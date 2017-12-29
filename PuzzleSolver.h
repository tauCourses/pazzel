#ifndef PUZZLE_PUZZLESOLVER_H
#define PUZZLE_PUZZLESOLVER_H


#include <vector>
#include <memory>
#include <fstream>
#include <deque>
#include <climits>
#include <thread>
#include <mutex>
#include <algorithm>

#include "AbstractPieceManager.h"
#include "PuzzlePieceConstrain.h"

using namespace std;

#define STEP_TO_CHECK_MUTEX 1000
class PuzzleSolver {
private:
    struct PuzzleLocation {
        int row, col;
    };

    struct PuzzlePieceData {
        PuzzleLocation location;
        Piece_t current;
    };

    class ThreadRun{
    public:
        explicit ThreadRun(unique_ptr<AbstractPieceManager> &pieceManager);

        vector<vector<Piece_t>> puzzleSolution;
        vector<vector<Piece_t>> puzzleConstrain;
        AbstractPieceManager::Shape shape;
        unique_ptr<AbstractPieceManager> &pieceManager;
        int stepCounter = 0;
    };

    int numberOfThreads;
    //shared data for all of threads:
    vector<AbstractPieceManager::Shape> allPossiblePuzzleShapes;
    bool solutionFound = false;
    vector<vector<Piece_t>> puzzleSolution;
    mutex globalDataMutex; //mutex to get access to global data

    const unique_ptr<AbstractPieceManager> &pieceManager;
    mutex pieceManagerMutex;

    void createNewPuzzleSolution(ThreadRun &run);

    bool trySolveForShape(ThreadRun &run);

    bool tryGettingNextPuzzleLocationToFill(ThreadRun &run, PuzzleLocation &bestLocation);

    void updatePieceInSolution(ThreadRun &run, PuzzleLocation puzzleLocation, Piece_t currentPiece);

    void removePieceFromSolution(ThreadRun &run, PuzzleLocation puzzleLocation);

    void runThread();

    static inline Piece_t getConstrainOpposite(Piece_t currentConstrain);

public:

    explicit PuzzleSolver(const unique_ptr<AbstractPieceManager> &pieceManager, int numberOfThreads);

    bool trySolve();

    void exportSolution(ofstream &out);

    void exportErrors(ofstream &out);

    bool isSolutionFound();
};


#endif //PUZZLE_PUZZLESOLVER_H