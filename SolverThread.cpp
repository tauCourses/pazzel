#include "SolverThread.h"

SolverThread::SolverThread(SharedData& threadData,const unique_ptr<AbstractPieceManager>& globalPieceManager, ThreadType type):
        type(type), sharedData(threadData) {
    this->numberOfPieces = globalPieceManager->getNumberOfPieces();
    this->pieceManager = globalPieceManager->clone();
    this->pieceManager->retrieveData(globalPieceManager);
    this->sharedData.sharedDataMutex.lock();
    this->sharedData.sharedDataMutex.unlock();
}

void SolverThread::retrievePieceManager(const unique_ptr<AbstractPieceManager> &globalPieceManager) {
    this->pieceManager->retrieveData(globalPieceManager);
}

void SolverThread::exportThreadSolution(vector<vector<Piece_t>>& globalPuzzleSolution) {
    this->sharedData.sharedDataMutex.lock();
    this->sharedData.solutionFound = true;
    this->sharedData.terminateThreads = true; //stop all threads
    globalPuzzleSolution = this->puzzleSolution; //should be under global mutex!
    this->sharedData.sharedDataMutex.unlock();
}

bool SolverThread::trySolve() {
    this->stepCounter = 0;
    this->randomStepsCounter = 0;
    this->currentPiece = 0;
    createNewPuzzleSolution();
    this->locationsStack = std::stack<PuzzleLocation>();

    if (this->type == ThreadType::RANDOM) {
        this->fillRandomPieces();
        if(this->locationsStack.size() == this->numberOfPieces) //if found solution randomly, very unlikely
            return true;
    }
    while (!this->isThreadShouldEnd()) {
        if (!this->tryFindNextLocation())
            return false; //return false when arriving at the "end" of all possible solutions.
        if (this->fillCurrentPiece()) //return if puzzle completed
            return true;
    }
    return false;
}

bool SolverThread::isThreadShouldEnd() {
    ++this->stepCounter;
    if (this->stepCounter >= STEP_TO_CHECK_MUTEX) {
        this->sharedData.sharedDataMutex.lock();
        bool terminateThreads = this->sharedData.terminateThreads;
        this->sharedData.sharedDataMutex.unlock();
        if (terminateThreads)
            return true;
        if (this->type == ThreadType::RANDOM) {
            if (this->randomStepsCounter >= RANDOM_RUN_LIMIT) {
                return true;
            }
            ++this->randomStepsCounter;
        }
        this->stepCounter = 0;
    }
    return false;
}

bool SolverThread::fillCurrentPiece() {
    auto currentConstrain = this->puzzleConstrain[this->currentLocation.row][this->currentLocation.col];
    this->currentPiece = this->pieceManager->getNextPiece(currentConstrain, this->currentPiece);
    if (this->currentPiece == nullPiece)
        return false;

    locationsStack.push(this->currentLocation);
    this->updatePieceInSolution();
    this->puzzleSolution[this->currentLocation.row][this->currentLocation.col] = this->currentPiece;
    return this->numberOfPieces == this->locationsStack.size(); //return if puzzle solved!
}

bool SolverThread::tryFindNextLocation() {
    //on the way in:
    if (currentPiece != nullPiece && this->tryGettingNextPuzzleLocationToFill()) {
        currentPiece = nullPiece;
        return true;
    }
    //on the way out:
    if (locationsStack.empty())
        return false; // no solution;

    this->currentLocation = locationsStack.top();
    locationsStack.pop();
    currentPiece = this->puzzleSolution[this->currentLocation.row][this->currentLocation.col];
    this->removePieceFromSolution();
    return true;
}

void SolverThread::createNewPuzzleSolution()  {
    this->puzzleSolution.clear();
    this->puzzleConstrain.clear();

    for (int i = 0; i < this->shape.height; i++) {
        this->puzzleSolution.emplace_back(this->shape.width, nullPiece);
        this->puzzleConstrain.emplace_back(this->shape.width, nullPiece);
    }
    for (int i = 0; i < this->shape.width; i++) {
        this->puzzleConstrain[0][i] &= hasUpperStraight;
        this->puzzleConstrain[this->shape.height - 1][i] &= hasDownStraight;
    }
    for (int i = 0; i < this->shape.height; i++) {
        this->puzzleConstrain[i][0] &= hasLeftStraight;
        this->puzzleConstrain[i][this->shape.width - 1] &= hasRightStraight;
    }

}

void SolverThread::removePieceFromSolution() {
    this->puzzleSolution[this->currentLocation.row][this->currentLocation.col] = nullPiece;
    if (this->currentLocation.row > 0)
        this->puzzleConstrain[this->currentLocation.row - 1][this->currentLocation.col] |= 0x3 << 0;

    if (this->currentLocation.col > 0)
        this->puzzleConstrain[this->currentLocation.row][this->currentLocation.col - 1] |= 0x3 << 2;

    if (this->currentLocation.row < static_cast<int>(this->puzzleConstrain.size()) - 1)
        this->puzzleConstrain[currentLocation.row + 1][this->currentLocation.col] |= 0x3 << 4;

    if (this->currentLocation.col < static_cast<int>(this->puzzleConstrain[0].size()) - 1)
        this->puzzleConstrain[this->currentLocation.row][this->currentLocation.col + 1] |= 0x3 << 6;

}

void SolverThread::updatePieceInSolution()  {
    this->puzzleSolution[this->currentLocation.row][this->currentLocation.col] = this->currentPiece;
    if (currentLocation.row > 0) {
        this->puzzleConstrain[(this->currentLocation.row - 1)][this->currentLocation.col] &=
                (AbstractPieceManager::getConstrainOpposite(static_cast<Piece_t>((this->currentPiece >> 4) & 0x3)) << 0) |
                ((0x3 << 0) ^ nullPiece);
    }
    if (this->currentLocation.col > 0) {
        this->puzzleConstrain[this->currentLocation.row][this->currentLocation.col - 1] &=
                (AbstractPieceManager::getConstrainOpposite(static_cast<Piece_t>((this->currentPiece >> 6) & 0x3)) << 2) |
                ((0x3 << 2) ^ nullPiece);
    }
    if (this->currentLocation.row < static_cast<int>(this->puzzleConstrain.size()) - 1) {
        this->puzzleConstrain[(this->currentLocation.row + 1)][this->currentLocation.col] &=
                (AbstractPieceManager::getConstrainOpposite(static_cast<Piece_t>((this->currentPiece >> 0) & 0x3)) << 4) |
                ((0x3 << 4) ^ nullPiece);
    }
    if (this->currentLocation.col < static_cast<int>(this->puzzleConstrain[0].size()) - 1) {
        this->puzzleConstrain[this->currentLocation.row][this->currentLocation.col + 1] &=
                (AbstractPieceManager::getConstrainOpposite(static_cast<Piece_t>((this->currentPiece >> 2) & 0x3)) << 6) |
                ((0x3 << 6) ^ nullPiece);
    }
}

bool SolverThread::tryGettingNextPuzzleLocationToFill() {
    int minOption = INT_MAX;
    for (int currentRow = 0; currentRow < this->shape.height; ++currentRow) {
        for (int currentCol = 0; currentCol < this->shape.width; ++currentCol) {
            if (nullPiece == this->puzzleSolution[currentRow][currentCol]) {
                int currentConstrainCount = this->pieceManager->numOfOptionsForConstrain(
                        this->puzzleConstrain[currentRow][currentCol]);
                if (currentConstrainCount < minOption) {
                    if (currentConstrainCount == 0) {
                        return false; // no available piece for a location, should go back.
                    } else {
                        minOption = currentConstrainCount;
                        this->currentLocation.row = currentRow;
                        this->currentLocation.col = currentCol;
                    }
                }
            }
        }
    }
    return true;
}

void SolverThread::fillRandomPieces() {
    if (!this->tryGettingNextPuzzleLocationToFill())
        return;
    do {
        setRandomPiece();

        locationsStack.push(this->currentLocation);
        this->updatePieceInSolution();
        this->puzzleSolution[this->currentLocation.row][this->currentLocation.col] = this->currentPiece;

        if (this->numberOfPieces == this->locationsStack.size())
            return;
    } while (this->tryGettingNextPuzzleLocationToFill());
}

void SolverThread::setRandomPiece()  {
    auto currentConstrain = this->puzzleConstrain[this->currentLocation.row][this->currentLocation.col];
    unsigned long numberOfPossiblePieces = 0;
    for (uint8_t piece = this->pieceManager->getNextPiece(currentConstrain, nullPiece); piece != nullPiece;
         piece = this->pieceManager->getNextPiece(currentConstrain, piece))
        numberOfPossiblePieces++;
    auto randomPieceIndex = static_cast<int>((rand() % numberOfPossiblePieces) + 1);
    this->currentPiece = nullPiece;
    for (int i = 0; i < randomPieceIndex; i++)
        this->currentPiece = this->pieceManager->getNextPiece(currentConstrain, this->currentPiece);
}

