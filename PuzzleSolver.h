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
        int row, col;
    };

    struct PuzzlePieceData {
        PuzzleLocation location;
        Piece_t current;
    };

    class ThreadData{
    public:
        explicit ThreadData(unique_ptr<AbstractPieceManager> &pieceManager, int numberOfPieces, int id);

        vector<vector<Piece_t>> puzzleSolution;
        vector<vector<Piece_t>> puzzleConstrain;
        AbstractPieceManager::Shape shape;
        unique_ptr<AbstractPieceManager> &pieceManager;
        bool randomize = false;
        int stepCounter = 0; //
        int randomStepsCounter = 0;
        int id = 0;
        int numberOfPieces = 0;
    };

    int numberOfThreads;
    //shared data for all of threads:
    vector<AbstractPieceManager::Shape> allPossiblePuzzleShapes;
    vector<AbstractPieceManager::Shape> ShapesForSerialScanning;
    bool solutionFound = false;
    vector<vector<Piece_t>> puzzleSolution;
    mutex globalDataMutex; //mutex to get access to global data

    const unique_ptr<AbstractPieceManager> &pieceManager;
    int threadsCounter = 1;
    int numberOfPieces;
    mutex pieceManagerMutex;

    void createNewPuzzleSolution(ThreadData &threadData);

    bool trySolveForThread(ThreadData &threadData);

    bool tryGettingNextPuzzleLocationToFill(ThreadData &threadData, PuzzleLocation &bestLocation);

    void updatePieceInSolution(ThreadData &threadData, PuzzleLocation puzzleLocation, Piece_t currentPiece);

    void removePieceFromSolution(ThreadData &threadData, PuzzleLocation puzzleLocation);

    void runThread();

    static inline Piece_t getConstrainOpposite(Piece_t currentConstrain);

    void initPuzzleShapesVectors();

    bool tryInitNextThreadRun(ThreadData &threadData);
    void initSerialRun(ThreadData &threadData);
    void initRandomizeRun(ThreadData &threadData);
    void exportThreadSolution(ThreadData &threadData);

public:

    explicit PuzzleSolver(const unique_ptr<AbstractPieceManager> &pieceManager, int numberOfThreads);

    bool trySolve();

    void exportSolution(ofstream &out);

    void exportErrors(ofstream &out);

    bool isSolutionFound();


    bool isThreadShouldEnd(ThreadData &threadData);

    bool tryFindNextLocation(ThreadData &data, stack<PuzzleLocation> &stack, PuzzlePieceData &pieceData);

    bool fillCurrentPiece(ThreadData &threadData, stack<PuzzleLocation> &stack, PuzzlePieceData &currentPiec);

    bool trySetRandomizePossition(ThreadData &threadData, stack<PuzzleLocation> &stack, PuzzlePieceData &currentPiece);

    void setRandomPiece(ThreadData &threadData, PuzzlePieceData &currentPiece);

    void retrieveThreadPieceManager(ThreadData &threadData);
};


#endif //PUZZLE_PUZZLESOLVER_H