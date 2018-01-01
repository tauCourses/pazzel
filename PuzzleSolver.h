#ifndef PUZZLE_PUZZLESOLVER_H
#define PUZZLE_PUZZLESOLVER_H


#include <vector>
#include <stack>
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

#define STEP_TO_CHECK_MUTEX 5000
#define RANDOM_RUN_LIMIT 150

class PuzzleSolver {
private:
    struct PuzzleLocation {
        int row = 0, col = 0;
    };

    struct PuzzlePieceData {
        PuzzleLocation location;
        Piece_t current = 0;
    };

    class ThreadData {
    public:
        explicit ThreadData(bool randomize, int id,
                            int numberOfPieces);

        vector<vector<Piece_t>> puzzleSolution;
        vector<vector<Piece_t>> puzzleConstrain;
        AbstractPieceManager::Shape shape;
        unique_ptr<AbstractPieceManager> pieceManager;
        bool randomize = false;
        int stepCounter = 0;
        int randomStepsCounter = 0;
        int id = 0;
        int numberOfPieces = 0;
    };

    //shared data for all of threads:
    vector<AbstractPieceManager::Shape> allPossiblePuzzleShapes;
    vector<AbstractPieceManager::Shape> shapesForSerialScanning;
    bool solutionFound = false;
    bool noSolutionExist = false;
    vector<vector<Piece_t>> puzzleSolution;
    mutex globalDataMutex; //mutex to get access to global data

    const unique_ptr<AbstractPieceManager> &prototypePieceManager;
    int threadsCounter = 1;
    int numberOfThreads;
    int numberOfPieces;
    mutex pieceManagerMutex;

    void createNewPuzzleSolution(ThreadData &threadData) const;

    bool trySolveForThread(ThreadData &threadData);

    bool tryGettingNextPuzzleLocationToFill(ThreadData &threadData, PuzzleLocation &bestLocation) const;

    void updatePieceInSolution(ThreadData &threadData, PuzzleLocation puzzleLocation, Piece_t currentPiece) const;

    void removePieceFromSolution(ThreadData &threadData, PuzzleLocation puzzleLocation) const;

    void runRandomThread();

    void runSerialThread();

    void runThread(ThreadData &threadData);

    inline Piece_t getConstrainOpposite(Piece_t currentConstrain) const;

    void initPuzzleShapesVectors();

    bool tryInitNextThreadRun(ThreadData &threadData);

    void initSerialRun(ThreadData &threadData);

    void initRandomizeRun(ThreadData &threadData) const;

    void exportThreadSolution(ThreadData &threadData);

    bool tryFindNextLocation(ThreadData &data, stack<PuzzleLocation> &stack, PuzzlePieceData &pieceData) const;

    bool fillCurrentPiece(ThreadData &threadData, stack<PuzzleLocation> &stack, PuzzlePieceData &currentPiec) const;

    bool
    tryFillRandomPieces(ThreadData &threadData, stack<PuzzleLocation> &stack, PuzzlePieceData &currentPiece) const;

    void setRandomPiece(ThreadData &threadData, PuzzlePieceData &currentPiece) const;

    bool isSolutionFound();

    bool isThreadShouldEnd(ThreadData &threadData);

public:

    explicit PuzzleSolver(const unique_ptr<AbstractPieceManager> &pieceManager, int numberOfThreads);

    bool trySolve();

    void exportSolution(ofstream &out) const;

    void exportErrors(ofstream &out) const;

};


#endif //PUZZLE_PUZZLESOLVER_H