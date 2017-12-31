#include "PuzzleSolver.h"

#include <random>
#include "BasicPieceManager.h"

PuzzleSolver::PuzzleSolver(const unique_ptr<AbstractPieceManager> &pieceManager, int numberOfThreads) :
        pieceManager(pieceManager)
{
   /* int hardwareConcurrency = std::thread::hardware_concurrency(); //TODO->decide if to remove it or uncomment
    if(hardwareConcurrency != 0)
        this->numberOfThreads = std::min(hardwareConcurrency, numberOfThreads);*/
    this->numberOfThreads = numberOfThreads;
    this->numberOfPieces = this->pieceManager->getNumberOfPieces();
    srand((unsigned)time(0)); //seed for random genrator
}


PuzzleSolver::ThreadData::ThreadData(unique_ptr<AbstractPieceManager> &pieceManager, int numberOfPieces, int id) :
        pieceManager(pieceManager)
{
    this->numberOfPieces = numberOfPieces;
    this->id = id;
}

bool PuzzleSolver::trySolve() {
    if(!this->pieceManager->preConditions()) //check pre-conditions
        return false;

    this->initPuzzleShapesVectors();

    vector<std::thread> threadVector;
    for(int i=0; i<this->numberOfThreads-1;++i) //one thread is the main thread
        threadVector.emplace_back(std::thread(&PuzzleSolver::runThread, this));

    runThread();
    for (auto &threadRun : threadVector)
        if(threadRun.joinable())
            threadRun.join();

    return this->solutionFound;
}

void PuzzleSolver::initPuzzleShapesVectors() {
    this->allPossiblePuzzleShapes = this->pieceManager->getAllPossiblePuzzleShapes();
    this->ShapesForSerialScanning = this->allPossiblePuzzleShapes;
    auto rng = std::default_random_engine {};
    rng.seed(unsigned(time(nullptr)));
    std::shuffle(std::begin(this->ShapesForSerialScanning), std::end(this->ShapesForSerialScanning), rng);
}

void PuzzleSolver::runThread() {
    this->pieceManagerMutex.lock();
    unique_ptr<AbstractPieceManager> tempPieceManager = this->pieceManager->clone();
    int numberOfPieces = this->numberOfPieces;
    int id = this->threadsCounter++;
    this->pieceManagerMutex.unlock();
    ThreadData threadData(tempPieceManager, numberOfPieces, id);

    while(true)
    {
        if(!tryInitNextThreadRun(threadData))
            break;

        if(trySolveForThread(threadData))
        {
            this->exportThreadSolution(threadData);
            break;
        }
        else if(threadData.randomize)
                cout << "thread " << threadData.id << " end random run" << endl << std::flush;
        else
            cout << "thread " << threadData.id << " end serial run" << endl << std::flush;
    }
    cout << "thread " << threadData.id << " died" << endl << std::flush;
}

bool PuzzleSolver::tryInitNextThreadRun(PuzzleSolver::ThreadData &threadData) {
    this->globalDataMutex.lock();
    if(this->solutionFound) {
        this->globalDataMutex.unlock();
        return false;
    }
    //if(!this->ShapesForSerialScanning.empty())
    //    this->initSerialRun(threadData);
    //else
        this->initRandomizeRun(threadData);
    this->globalDataMutex.unlock();
    if(threadData.randomize)
        cout << "random ";
    cout << "thread " << threadData.id << " start scanning shpae "
         << threadData.shape.width << ", " << threadData.shape.height << endl << std::flush;
    return true;
}

void PuzzleSolver::initSerialRun(ThreadData &threadData) //globalDataMutex has to be locked!
{
    threadData.shape = this->ShapesForSerialScanning.back();
    this->ShapesForSerialScanning.pop_back();
    threadData.randomize = false;
    threadData.stepCounter = 0; //start counter from begining
}

void PuzzleSolver::initRandomizeRun(ThreadData &threadData) //globalDataMutex has to be locked!
{
    unsigned long random_integer = (rand() % this->allPossiblePuzzleShapes.size());
    threadData.shape = this->allPossiblePuzzleShapes.at(random_integer);
    threadData.randomize = true;
    threadData.stepCounter = 0;
    threadData.randomStepsCounter = 0;
}

void PuzzleSolver::exportThreadSolution(PuzzleSolver::ThreadData &threadData) {
    cout << "thread " << threadData.id << " found solution for shpae "
         << threadData.shape.width << ", " << threadData.shape.height <<endl << std::flush;
    this->globalDataMutex.lock();
    this->solutionFound = true;
    this->puzzleSolution = threadData.puzzleSolution;
    this->globalDataMutex.unlock();

}

bool PuzzleSolver::trySolveForThread(ThreadData &threadData) {
    createNewPuzzleSolution(threadData);
    stack<PuzzleLocation> puzzleLocationStack;
    PuzzlePieceData currentPiece;

    if(threadData.randomize) {
        if (!this->trySetRandomizePossition(threadData, puzzleLocationStack, currentPiece))
            return false;
    }else if(this->tryGettingNextPuzzleLocationToFill(threadData, currentPiece.location))
        currentPiece.current = nullPiece;
    else
        return false;

    while (true) {
        if(this->isThreadShouldEnd(threadData))
            return false;

        if(this->fillCurrentPiece(threadData,puzzleLocationStack,currentPiece)) //return if puzzle completed
            return true;

        if(!this->tryFindNextLocation(threadData, puzzleLocationStack, currentPiece)) //return if found next location
            return false;
    }
}

bool PuzzleSolver::isThreadShouldEnd(ThreadData &threadData) {
    threadData.stepCounter++;
    if(threadData.stepCounter == STEP_TO_CHECK_MUTEX)
    {
        if(this->isSolutionFound())
            return true;
        if(threadData.randomize)
        {
            if(threadData.randomStepsCounter == RANDOM_RUN_LIMIT)
            {
                this->retrieveThreadPieceManager(threadData);
                return true;
            }
            threadData.randomStepsCounter++;
        }
        threadData.stepCounter = 0;
    }
    return false;
}

bool PuzzleSolver::fillCurrentPiece(ThreadData &threadData, stack <PuzzleLocation> &stack, PuzzlePieceData &currentPiece) {
    auto currentConstrain = threadData.puzzleConstrain[currentPiece.location.row][currentPiece.location.col];
    currentPiece.current = threadData.pieceManager->getNextPiece(currentConstrain, currentPiece.current);
    if (currentPiece.current == nullPiece)
        return false;

    stack.push(currentPiece.location);
    this->updatePieceInSolution(threadData, currentPiece.location, currentPiece.current);
    threadData.puzzleSolution[currentPiece.location.row][currentPiece.location.col] = currentPiece.current;

    return threadData.numberOfPieces == static_cast<int>(stack.size()); //return if puzzle solved!
}

bool PuzzleSolver::tryFindNextLocation(ThreadData &threadData, stack<PuzzleLocation> &stack, PuzzlePieceData &currentPiece) {
    if (currentPiece.current == nullPiece) //we are on the way out
    {
        if (stack.empty())
            return false; // no solution;
    }
    else if (this->tryGettingNextPuzzleLocationToFill(threadData, currentPiece.location)) //we are on the way in
    {
        currentPiece.current = nullPiece;
        return true;
    }

    currentPiece.location = stack.top();
    stack.pop();
    currentPiece.current = threadData.puzzleSolution[currentPiece.location.row][currentPiece.location.col];
    this->removePieceFromSolution(threadData, currentPiece.location);
    return true;
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

void PuzzleSolver::createNewPuzzleSolution(ThreadData &threadData) {
    threadData.puzzleSolution.clear();
    threadData.puzzleConstrain.clear();

    for (int i = 0; i < threadData.shape.height; i++) {
        threadData.puzzleSolution.emplace_back(threadData.shape.width, nullPiece);
        threadData.puzzleConstrain.emplace_back(threadData.shape.width, nullPiece);
    }
    for (int i = 0; i < threadData.shape.width; i++) {
        threadData.puzzleConstrain[0][i] &= hasUpperStraight;
        threadData.puzzleConstrain[threadData.shape.height - 1][i] &= hasDownStraight;
    }
    for (int i = 0; i < threadData.shape.height; i++) {
        threadData.puzzleConstrain[i][0] &= hasLeftStraight;
        threadData.puzzleConstrain[i][threadData.shape.width - 1] &= hasRightStraight;
    }

}

void PuzzleSolver::removePieceFromSolution(ThreadData &threadData, PuzzleSolver::PuzzleLocation puzzleLocation) {
    threadData.puzzleSolution[puzzleLocation.row][puzzleLocation.col] = nullPiece;
    if (puzzleLocation.row > 0)
        threadData.puzzleConstrain[puzzleLocation.row - 1][puzzleLocation.col] |= 0x3 << 0;

    if (puzzleLocation.col > 0)
        threadData.puzzleConstrain[puzzleLocation.row][puzzleLocation.col - 1] |= 0x3 << 2;

    if (puzzleLocation.row < static_cast<int>(threadData.puzzleConstrain.size()) - 1)
        threadData.puzzleConstrain[puzzleLocation.row + 1][puzzleLocation.col] |= 0x3 << 4;

    if (puzzleLocation.col < static_cast<int>(threadData.puzzleConstrain[0].size()) - 1)
        threadData.puzzleConstrain[puzzleLocation.row][puzzleLocation.col + 1] |= 0x3 << 6;

}

void PuzzleSolver::updatePieceInSolution(ThreadData &threadData, PuzzleSolver::PuzzleLocation puzzleLocation, Piece_t currentPiece) {
    threadData.puzzleSolution[puzzleLocation.row][puzzleLocation.col] = currentPiece;
    if (puzzleLocation.row > 0) {
        threadData.puzzleConstrain[(puzzleLocation.row - 1)][puzzleLocation.col] &=
                (PuzzleSolver::getConstrainOpposite(static_cast<Piece_t>((currentPiece >> 4) & 0x3)) << 0) |
                ((0x3 << 0) ^ nullPiece);
    }
    if (puzzleLocation.col > 0) {
        threadData.puzzleConstrain[puzzleLocation.row][puzzleLocation.col - 1] &=
                (PuzzleSolver::getConstrainOpposite(static_cast<Piece_t>((currentPiece >> 6) & 0x3)) << 2) |
                ((0x3 << 2) ^ nullPiece);
    }
    if (puzzleLocation.row < static_cast<int>(threadData.puzzleConstrain.size()) - 1) {
        threadData.puzzleConstrain[(puzzleLocation.row + 1)][puzzleLocation.col] &=
                (PuzzleSolver::getConstrainOpposite(static_cast<Piece_t>((currentPiece >> 0) & 0x3)) << 4) |
                ((0x3 << 4) ^ nullPiece);
    }
    if (puzzleLocation.col < static_cast<int>(threadData.puzzleConstrain[0].size()) - 1) {
        threadData.puzzleConstrain[puzzleLocation.row][puzzleLocation.col + 1] &=
                (PuzzleSolver::getConstrainOpposite(static_cast<Piece_t>((currentPiece >> 2) & 0x3)) << 6) |
                ((0x3 << 6) ^ nullPiece);
    }
}

bool PuzzleSolver::tryGettingNextPuzzleLocationToFill(ThreadData &threadData,PuzzleLocation &bestLocation) {
    int minOption = INT_MAX;
    for (int currentRow = 0; currentRow < threadData.shape.height; ++currentRow) {
        for (int currentCol = 0; currentCol < threadData.shape.width; ++currentCol) {
            if (nullPiece == threadData.puzzleSolution[currentRow][currentCol]) {
                int currentConstrainCount = threadData.pieceManager->numOfOptionsForConstrain(
                        threadData.puzzleConstrain[currentRow][currentCol]);
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

void PuzzleSolver::exportErrors(ofstream &out) {
    out << "Cannot solve puzzle: it seems that there is no proper solution" << "\n";
}

bool PuzzleSolver::trySetRandomizePossition(ThreadData &threadData, stack <PuzzleLocation> &stack,
                                            PuzzlePieceData &currentPiece) {
    if(!this->tryGettingNextPuzzleLocationToFill(threadData, currentPiece.location))
        return false;

    do
    {
        setRandomPiece(threadData, currentPiece);

        stack.push(currentPiece.location);
        this->updatePieceInSolution(threadData, currentPiece.location, currentPiece.current);
        threadData.puzzleSolution[currentPiece.location.row][currentPiece.location.col] = currentPiece.current;

        if(threadData.numberOfPieces == static_cast<int>(stack.size()))
            break;

    }while(this->tryGettingNextPuzzleLocationToFill(threadData, currentPiece.location));

    currentPiece.location = stack.top();
    stack.pop();
    currentPiece.current = threadData.puzzleSolution[currentPiece.location.row][currentPiece.location.col];
    this->removePieceFromSolution(threadData, currentPiece.location);
    cout << "random thread " << threadData.id << " starting from random position at depth " << stack.size() << endl << std::flush;
    return true;
}

void PuzzleSolver::setRandomPiece(ThreadData &threadData, PuzzlePieceData &currentPiece) {
    auto currentConstrain = threadData.puzzleConstrain[currentPiece.location.row][currentPiece.location.col];
    unsigned long numberOfPossiblePieces=0;
    for(uint8_t piece = threadData.pieceManager->getNextPiece(currentConstrain,nullPiece); piece != nullPiece;
        piece = threadData.pieceManager->getNextPiece(currentConstrain,piece))
        numberOfPossiblePieces++;

    int randomPieceIndex = static_cast<int>((rand() % numberOfPossiblePieces) + 1);
    currentPiece.current = nullPiece;
    for(int i=0;i<randomPieceIndex;i++)
        currentPiece.current = threadData.pieceManager->getNextPiece(currentConstrain,currentPiece.current);
}

void PuzzleSolver::retrieveThreadPieceManager(PuzzleSolver::ThreadData &threadData) {
    this->pieceManagerMutex.lock();
    threadData.pieceManager->retrieveData(this->pieceManager);
    this->pieceManagerMutex.unlock();
}


