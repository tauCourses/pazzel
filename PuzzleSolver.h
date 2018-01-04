#ifndef PUZZLE_PUZZLESOLVER_H
#define PUZZLE_PUZZLESOLVER_H

#include <vector>
#include <memory>
#include <fstream>
#include <thread>
#include <mutex>
#include <random>

#include "SolverThread.h"
#include "PuzzleException.h"

#define RANDOM_TRIES_BEFORE_SERIAL 10

using namespace std;

class PuzzleSolver {
private:

    SolverThread::SharedData threadsSharedData;

    vector<AbstractPieceManager::Shape> allPossiblePuzzleShapes;
    vector<AbstractPieceManager::Shape> shapesForSerialScanning;
    const unique_ptr<AbstractPieceManager> &pieceManager;
    vector<vector<Piece_t>> puzzleSolution;
    mutex puzzleSolverMutex; //mutex for pieceManager and  threadsCounter and numberOfPieces

    SolverThread::SharedData sharedData;

    int numberOfThreads;

    void runThread(ThreadType type);

    void runSingleThread(); //a unique approach for single thread solution

    void initPuzzleShapesVectors();

    bool tryInitNextThreadRun(SolverThread &solverThread);

    bool tryInitSerialRun(SolverThread &solverThread);

    void initRandomizeRun(SolverThread &solverThread);

public:
    explicit PuzzleSolver(const unique_ptr<AbstractPieceManager> &pieceManager, int numberOfThreads);

    void solve();

    void exportSolution(ofstream &out) const;
};


#endif //PUZZLE_PUZZLESOLVER_H