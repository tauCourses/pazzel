#include "PuzzleSolver.h"
#include "BasicPieceManager.h"

PuzzleSolver::PuzzleSolver(const unique_ptr<AbstractPieceManager> &pieceManager, int numberOfThreads) :
        pieceManager(pieceManager)
{
    int hardwareConcurrency = std::thread::hardware_concurrency();
    cout << "hardware threads: " << hardwareConcurrency << endl; //TODO->remove!
    if(numberOfThreads != 0)
        this->numberOfThreads = 1;//std::min(hardwareConcurrency, numberOfThreads);
}


PuzzleSolver::ThreadRun::ThreadRun(unique_ptr<AbstractPieceManager> &pieceManager) :
        pieceManager(pieceManager) {}

bool PuzzleSolver::trySolve() {
    if(!this->pieceManager->preConditions()) //check pre-conditions
        return false;

    this->allPossiblePuzzleShapes = pieceManager->getAllPossiblePuzzleShapes();
    std::random_shuffle(allPossiblePuzzleShapes.begin(), allPossiblePuzzleShapes.end());

    vector<std::thread> threadVector;

   // for(int i=0; i<this->numberOfThreads-1;++i) //one thread is the main thread
     //   threadVector.emplace_back(std::thread(&PuzzleSolver::runThread, this));

    runThread();
    for (auto &threadRun : threadVector)
        if(threadRun.joinable())
            threadRun.join();

    return this->solutionFound;
}

void PuzzleSolver::runThread() {

    this->pieceManagerMutex.lock();
    unique_ptr<AbstractPieceManager> tempPieceManager = this->pieceManager->clone();
    this->pieceManagerMutex.unlock();
    ThreadRun threadData(tempPieceManager);
    while(true)
    {
        this->globalDataMutex.lock();
        if(this->solutionFound || this->allPossiblePuzzleShapes.empty()) {
            this->globalDataMutex.unlock();
            break;
        }
        threadData.shape = this->allPossiblePuzzleShapes.back();
        this->allPossiblePuzzleShapes.pop_back();
        this->globalDataMutex.unlock();

        threadData.stepCounter = 0; //start counter from begining
        if(trySolveForShape(threadData))
        {
            this->globalDataMutex.lock();

            this->solutionFound = true;
            this->puzzleSolution = threadData.puzzleSolution;

            this->globalDataMutex.unlock();
            break;
        }
    }
}

bool PuzzleSolver::trySolveForShape(ThreadRun &run) {
    createNewPuzzleSolution(run);
    int numberOfPieces = run.shape.width * run.shape.height;
    vector<PuzzleLocation> puzzleLocationFilled(static_cast<unsigned long>(numberOfPieces));
    int puzzleLocationFilledSize = 0;
    PuzzlePieceData currentPiece;
    bool puzzleIsCompleted = false;
    bool needToGoBack = false;
    // loop while puzzle is not completed or no piece to fill in.
    while (!puzzleIsCompleted) {
        run.stepCounter++;
        if(run.stepCounter == STEP_TO_CHECK_MUTEX)
        {
            if(this->isSolutionFound())
                return false;
            run.stepCounter = 0;
        }
        if (needToGoBack) {
            needToGoBack = false;
            if (puzzleLocationFilledSize == 0)
                return false; // no solution;
            currentPiece.location = puzzleLocationFilled[--puzzleLocationFilledSize];
            currentPiece.current = run.puzzleSolution[currentPiece.location.row][currentPiece.location.col];
            this->removePieceFromSolution(run, currentPiece.location);
        }
        else {
            if (!this->tryGettingNextPuzzleLocationToFill(run, currentPiece.location)) {
                needToGoBack = true; // you screwed up something in the last move.
                continue;
            }
            currentPiece.current = nullPiece;
        }
        auto currentConstrain = run.puzzleConstrain[currentPiece.location.row][currentPiece.location.col];
        currentPiece.current = run.pieceManager->getNextPiece(currentConstrain, currentPiece.current);
        if (currentPiece.current == nullPiece)
            needToGoBack = true;
        else {
            puzzleLocationFilled[puzzleLocationFilledSize++] = currentPiece.location;
            this->updatePieceInSolution(run, currentPiece.location, currentPiece.current);
            run.puzzleSolution[currentPiece.location.row][currentPiece.location.col] = currentPiece.current;
        }
        puzzleIsCompleted = (numberOfPieces == puzzleLocationFilledSize);
    }
    return puzzleIsCompleted;
}

void PuzzleSolver::exportErrors(ofstream &out) {
    out << "Cannot solve puzzle: it seems that there is no proper solution" << "\n";
}

void PuzzleSolver::exportSolution(ofstream &out) {
    auto height = static_cast<int>(puzzleSolution.size());
    auto width = static_cast<int>(puzzleSolution[0].size());
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            this->pieceManager->printPiece(puzzleSolution[i][j], out);
            if (j + 1 != width)
                out << " ";
            else
                out << "\n";
        }
    }
}

void PuzzleSolver::createNewPuzzleSolution(ThreadRun &run) {
    run.puzzleSolution.clear();
    run.puzzleConstrain.clear();

    for (int i = 0; i < run.shape.height; i++) {
        run.puzzleSolution.emplace_back(run.shape.width, nullPiece);
        run.puzzleConstrain.emplace_back(run.shape.width, nullPiece);
    }
    for (int i = 0; i < run.shape.width; i++) {
        run.puzzleConstrain[0][i] &= hasUpperStraight;
        run.puzzleConstrain[run.shape.height - 1][i] &= hasDownStraight;
    }
    for (int i = 0; i < run.shape.height; i++) {
        run.puzzleConstrain[i][0] &= hasLeftStraight;
        run.puzzleConstrain[i][run.shape.width - 1] &= hasRightStraight;
    }

}

void PuzzleSolver::removePieceFromSolution(ThreadRun &run, PuzzleSolver::PuzzleLocation puzzleLocation) {
    run.puzzleSolution[puzzleLocation.row][puzzleLocation.col] = nullPiece;
    if (puzzleLocation.row > 0)
        run.puzzleConstrain[puzzleLocation.row - 1][puzzleLocation.col] |= 0x3 << 0;

    if (puzzleLocation.col > 0)
        run.puzzleConstrain[puzzleLocation.row][puzzleLocation.col - 1] |= 0x3 << 2;

    if (puzzleLocation.row < static_cast<int>(run.puzzleConstrain.size()) - 1)
        run.puzzleConstrain[puzzleLocation.row + 1][puzzleLocation.col] |= 0x3 << 4;

    if (puzzleLocation.col < static_cast<int>(run.puzzleConstrain[0].size()) - 1)
        run.puzzleConstrain[puzzleLocation.row][puzzleLocation.col + 1] |= 0x3 << 6;

}

void PuzzleSolver::updatePieceInSolution(ThreadRun &run, PuzzleSolver::PuzzleLocation puzzleLocation, Piece_t currentPiece) {
    run.puzzleSolution[puzzleLocation.row][puzzleLocation.col] = currentPiece;
    if (puzzleLocation.row > 0) {
        run.puzzleConstrain[(puzzleLocation.row - 1)][puzzleLocation.col] &=
                (PuzzleSolver::getConstrainOpposite(static_cast<Piece_t>((currentPiece >> 4) & 0x3)) << 0) |
                ((0x3 << 0) ^ nullPiece);
    }
    if (puzzleLocation.col > 0) {
        run.puzzleConstrain[puzzleLocation.row][puzzleLocation.col - 1] &=
                (PuzzleSolver::getConstrainOpposite(static_cast<Piece_t>((currentPiece >> 6) & 0x3)) << 2) |
                ((0x3 << 2) ^ nullPiece);
    }
    if (puzzleLocation.row < static_cast<int>(run.puzzleConstrain.size()) - 1) {
        run.puzzleConstrain[(puzzleLocation.row + 1)][puzzleLocation.col] &=
                (PuzzleSolver::getConstrainOpposite(static_cast<Piece_t>((currentPiece >> 0) & 0x3)) << 4) |
                ((0x3 << 4) ^ nullPiece);
    }
    if (puzzleLocation.col < static_cast<int>(run.puzzleConstrain[0].size()) - 1) {
        run.puzzleConstrain[puzzleLocation.row][puzzleLocation.col + 1] &=
                (PuzzleSolver::getConstrainOpposite(static_cast<Piece_t>((currentPiece >> 2) & 0x3)) << 6) |
                ((0x3 << 6) ^ nullPiece);
    }
}

bool PuzzleSolver::tryGettingNextPuzzleLocationToFill(ThreadRun &run,PuzzleLocation &bestLocation) {
    int minOption = INT_MAX;
    auto row = static_cast<int>(run.puzzleSolution.size());
    auto col = static_cast<int>(run.puzzleSolution[0].size());
    for (int currentRow = 0; currentRow < row; ++currentRow) {
        for (int currentCol = 0; currentCol < col; ++currentCol) {
            if (nullPiece == run.puzzleSolution[currentRow][currentCol]) {
                int currentConstrainCount = run.pieceManager->numOfOptionsForConstrain(
                        run.puzzleConstrain[currentRow][currentCol]);
                if (currentConstrainCount < minOption) {
                    if (currentConstrainCount == 0) {
                        return false; // no available piece for a location, should go back.
                    } else {
                        minOption = currentConstrainCount;
                        bestLocation.row = currentRow;
                        bestLocation.col = currentCol;
                    }
                }
            }
        }
    }
    return true;
}

inline Piece_t PuzzleSolver::getConstrainOpposite(Piece_t currentConstrain) {
    return static_cast<uint8_t>(
            (((currentConstrain & 0x1) == (uint8_t) (currentConstrain >> 1)) << 1) |
            (currentConstrain & 0x1));
}

bool PuzzleSolver::isSolutionFound() {
    this->globalDataMutex.lock();
    bool solutionFound = this->solutionFound;
    this->globalDataMutex.unlock();
    return solutionFound;
}


