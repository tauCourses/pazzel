#ifndef PUZZLE_SOLVERTHREAD_H
#define PUZZLE_SOLVERTHREAD_H

#include <vector>
#include <stack>
#include <memory>
#include <climits>
#include <deque>
#include <thread>
#include <mutex>
#include <algorithm>

#include "AbstractPieceManager.h"
#include "PuzzlePieceConstrain.h"

#define STEP_TO_CHECK_MUTEX 5000 //# of steps(aka,piece placements) before check if thread should terminated
#define RANDOM_RUN_LIMIT 150 //# of checking as above, before stop random thread that didn't find solution

enum class ThreadType {SERIAL, RANDOM};

class SolverThread {
public:
    struct SharedData {
        bool solutionFound = false;
        bool terminateThreads = false;
        std::mutex sharedDataMutex;
    };
    //puzzleSolverMutex should be lock when calling
    explicit SolverThread(SharedData& threadData,const unique_ptr<AbstractPieceManager>& globalPieceManager, ThreadType type);

    bool trySolve();

    //puzzleSolverMutex should be lock when calling
    void exportThreadSolution(vector<vector<Piece_t>>& puzzleSolution);

    //pieceManager need to be retrieved after random run, because it stops in the middle of the run
    //puzzleSolverMutex should be lock when calling
    void retrievePieceManager(const unique_ptr<AbstractPieceManager>& globalPieceManager);

    ThreadType type;
    AbstractPieceManager::Shape shape;
private:
    struct PuzzleLocation {
        int row = 0, col = 0;
    };

    SharedData& sharedData;
    vector<vector<Piece_t>> puzzleSolution;
    vector<vector<Piece_t>> puzzleConstrain;
    unique_ptr<AbstractPieceManager> pieceManager;
    int stepCounter = 0;
    int randomStepsCounter = 0;
    unsigned int numberOfPieces = 0;
    PuzzleLocation currentLocation;
    Piece_t currentPiece;
    stack<PuzzleLocation> locationsStack;

    void createNewPuzzleSolution();

    bool tryGettingNextPuzzleLocationToFill();

    void updatePieceInSolution();

    void removePieceFromSolution();

    bool tryFindNextLocation();

    bool fillCurrentPiece();

    void fillRandomPieces();

    void setRandomPiece();

    bool isThreadShouldEnd();
};

#endif //PUZZLE_SOLVERTHREAD_H
