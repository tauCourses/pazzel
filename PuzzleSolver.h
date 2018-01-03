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
    int numberOfPieces;

    void runThread(ThreadType type);

    void initPuzzleShapesVectors();

    bool tryInitNextThreadRun(SolverThread &solverThread);

    bool tryInitSerialRun(SolverThread &solverThread);

    void initRandomizeRun(SolverThread &solverThread);

public:
    explicit PuzzleSolver(const unique_ptr<AbstractPieceManager> &pieceManager, int numberOfThreads);

    void solve();

    void exportSolution(ofstream &out) const;

    void exportErrors(ofstream &out) const;

};


#endif //PUZZLE_PUZZLESOLVER_H