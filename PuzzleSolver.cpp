#include "PuzzleSolver.h"

#include <random>

PuzzleSolver::PuzzleSolver(const unique_ptr<AbstractPieceManager> &pieceManager,
                           const AbstractPieceManager::PieceRepository &prototypePiecesRepository, int numberOfThreads)
        : pieceManager(pieceManager), prototypePiecesRepository(prototypePiecesRepository),
          numberOfThreads(numberOfThreads), numberOfPieces(pieceManager->getNumberOfPieces()) {}

PuzzleSolver::ThreadData::ThreadData(bool randomize, int id, int numberOfPieces) :
        randomize(randomize), id(id), numberOfPieces(numberOfPieces) {}

bool PuzzleSolver::trySolve() {
    if (!this->pieceManager->preConditions()) //check pre-conditions
        return false;

    this->initPuzzleShapesVectors();

    vector<std::thread> randomThreadsVector;
    for (int i = 0; i < this->numberOfThreads - 1; ++i) //one thread is the main thread
        randomThreadsVector.emplace_back(std::thread(&PuzzleSolver::runRandomThread, this));


    runSerialThread();
    noSolutionExist = !solutionFound; //TODO ??
    for (auto &threadRun : randomThreadsVector)
        if (threadRun.joinable())
            threadRun.join();

    return this->solutionFound;
}

void PuzzleSolver::initPuzzleShapesVectors() {
    this->allPossiblePuzzleShapes = this->pieceManager->getAllPossiblePuzzleShapes();
    this->shapesForSerialScanning = this->allPossiblePuzzleShapes;
    auto rng = std::default_random_engine {};
    rng.seed(unsigned(time(nullptr)));
    std::shuffle(std::begin(this->shapesForSerialScanning), std::end(this->shapesForSerialScanning), rng);
}

void PuzzleSolver::runRandomThread() {
    this->pieceManagerMutex.lock();
    int numberOfPieces = this->numberOfPieces;
    int id = this->threadsCounter++;
    this->pieceManagerMutex.unlock();
    ThreadData threadData(true, id, numberOfPieces);
    runThread(threadData);
}

void PuzzleSolver::runSerialThread() {
    this->pieceManagerMutex.lock();
    int numberOfPieces = this->numberOfPieces;
    int id = this->threadsCounter++;
    this->pieceManagerMutex.unlock();
    ThreadData threadData(false, id, numberOfPieces);
    runThread(threadData);
}

void PuzzleSolver::runThread(ThreadData &threadData) {
    while (tryInitNextThreadRun(threadData)) {
        if (trySolveForThread(threadData)) {
            this->exportThreadSolution(threadData);
            break;
        } else if (threadData.randomize)
            cout << "thread " << threadData.id << " end random run" << endl << std::flush; // TODO delete
        else
            cout << "thread " << threadData.id << " end serial run" << endl << std::flush;// TODO delete
    }
    cout << "thread " << threadData.id << " died" << endl << std::flush;// TODO delete
}

bool PuzzleSolver::tryInitNextThreadRun(PuzzleSolver::ThreadData &threadData) {
    this->globalDataMutex.lock();
    if (this->solutionFound || this->noSolutionExist) {
        this->globalDataMutex.unlock();
        return false;
    }
    if (threadData.randomize) {
        this->initRandomizeRun(threadData);
    } else {
        if (shapesForSerialScanning.empty()) { // no more shapes to check
            this->globalDataMutex.unlock();
            return false;
        }
        this->initSerialRun(threadData);
    }
    this->globalDataMutex.unlock();
    if (threadData.randomize)// TODO delete
        cout << "random ";// TODO delete
    cout << "thread " << threadData.id << " start scanning shpae "// TODO delete
         << threadData.shape.width << ", " << threadData.shape.height << endl << std::flush;// TODO delete
    return true;
}

void PuzzleSolver::initSerialRun(ThreadData &threadData) //globalDataMutex has to be locked!
{
    threadData.shape = this->shapesForSerialScanning.back();
    this->shapesForSerialScanning.pop_back();
    threadData.stepCounter = 0; //start counter from beginning
}

void PuzzleSolver::initRandomizeRun(ThreadData &threadData) const//global Data Mutex has to be locked!
{
    unsigned long random_integer = (rand() % this->allPossiblePuzzleShapes.size());
    threadData.shape = this->allPossiblePuzzleShapes.at(random_integer);
    threadData.randomStepsCounter = 0;
}

void PuzzleSolver::exportThreadSolution(PuzzleSolver::ThreadData &threadData) {
    cout << "thread " << threadData.id << " found solution for shape "
         << threadData.shape.width << ", " << threadData.shape.height << endl << std::flush; // todo delete
    this->globalDataMutex.lock();
    this->solutionFound = true;
    this->puzzleSolution = threadData.puzzleSolution;
    this->globalDataMutex.unlock();
}

bool PuzzleSolver::trySolveForThread(ThreadData &threadData) {
    createNewPuzzleSolution(threadData);
    threadData.pieceRepository = this->prototypePiecesRepository;
    stack<PuzzleLocation> puzzleLocationStack;
    PuzzlePieceData currentPiece = {0, 0, 0};

    if (threadData.randomize) {
        if (!this->trySetRandomizePosition(threadData, puzzleLocationStack, currentPiece))
            return false;
    }

    while (!this->isThreadShouldEnd(threadData)) {
        if (!this->tryFindNextLocation(threadData, puzzleLocationStack, currentPiece))
            //return false when arriving at the "end" of all possible solutions.
            return false;

        if (this->fillCurrentPiece(threadData, puzzleLocationStack, currentPiece)) //return if puzzle completed
            return true;
    }
    return false;
}

bool PuzzleSolver::isThreadShouldEnd(ThreadData &threadData) {
    ++threadData.stepCounter;
    if (threadData.stepCounter >= STEP_TO_CHECK_MUTEX) {
        if (this->isSolutionFound())
            return true;
        if (threadData.randomize) {
            if (threadData.randomStepsCounter >= RANDOM_RUN_LIMIT) {
                return true;
            }
            ++threadData.randomStepsCounter;
        }
        threadData.stepCounter = 0;
    }
    return false;
}

bool PuzzleSolver::fillCurrentPiece(ThreadData &threadData, stack<PuzzleLocation> &stack,
                                    PuzzlePieceData &currentPiece) const {
    auto currentConstrain = threadData.puzzleConstrain[currentPiece.location.row][currentPiece.location.col];
    currentPiece.current = this->pieceManager->getNextPiece(threadData.pieceRepository, currentConstrain,
                                                            currentPiece.current);
    if (currentPiece.current == nullPiece)
        return false;

    stack.push(currentPiece.location);
    this->updatePieceInSolution(threadData, currentPiece.location, currentPiece.current);
    threadData.puzzleSolution[currentPiece.location.row][currentPiece.location.col] = currentPiece.current;
    return threadData.numberOfPieces == static_cast<int>(stack.size()); //return if puzzle solved!
}

bool PuzzleSolver::tryFindNextLocation(ThreadData &threadData, stack <PuzzleLocation> &stack,
                                       PuzzlePieceData &currentPiece) const {
    if (currentPiece.current != nullPiece && //we are on the way in
        this->tryGettingNextPuzzleLocationToFill(threadData, currentPiece.location)) {
        currentPiece.current = nullPiece;
        return true;
    }
    //we are on the way out
    if (stack.empty()) return false; // no solution;
    currentPiece.location = stack.top();
    stack.pop();
    currentPiece.current = threadData.puzzleSolution[currentPiece.location.row][currentPiece.location.col];
    this->removePieceFromSolution(threadData, currentPiece.location);
    return true;
}

void PuzzleSolver::exportSolution(ofstream &out) const {
    auto height = static_cast<int>(puzzleSolution.size());
    auto width = static_cast<int>(puzzleSolution[0].size());
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            this->pieceManager->printPiece(puzzleSolution[i][j], out);
            if (j + 1 != width)
                out << " ";
            else
                out << endl;
        }
    }
}

void PuzzleSolver::createNewPuzzleSolution(ThreadData &threadData) const {
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

void PuzzleSolver::removePieceFromSolution(ThreadData &threadData, PuzzleSolver::PuzzleLocation puzzleLocation) const {
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

void PuzzleSolver::updatePieceInSolution(ThreadData &threadData, PuzzleSolver::PuzzleLocation puzzleLocation,
                                         Piece_t currentPiece) const {
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

bool PuzzleSolver::tryGettingNextPuzzleLocationToFill(ThreadData &threadData, PuzzleLocation &bestLocation) const {
    int minOption = INT_MAX;
    for (int currentRow = 0; currentRow < threadData.shape.height; ++currentRow) {
        for (int currentCol = 0; currentCol < threadData.shape.width; ++currentCol) {
            if (nullPiece == threadData.puzzleSolution[currentRow][currentCol]) {
                int currentConstrainCount = this->pieceManager->numOfOptionsForConstrain(
                        threadData.pieceRepository, threadData.puzzleConstrain[currentRow][currentCol]);
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

inline Piece_t PuzzleSolver::getConstrainOpposite(Piece_t currentConstrain) const {
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

void PuzzleSolver::exportErrors(ofstream &out) const {
    out << "Cannot solve puzzle: it seems that there is no proper solution" << endl;
}

bool PuzzleSolver::trySetRandomizePosition(ThreadData &threadData, stack <PuzzleLocation> &stack,
                                           PuzzlePieceData &currentPiece) const {
    if (!this->tryGettingNextPuzzleLocationToFill(threadData, currentPiece.location))
        return false;

    do {
        setRandomPiece(threadData, currentPiece);

        stack.push(currentPiece.location);
        this->updatePieceInSolution(threadData, currentPiece.location, currentPiece.current);
        threadData.puzzleSolution[currentPiece.location.row][currentPiece.location.col] = currentPiece.current;

        if (threadData.numberOfPieces == static_cast<int>(stack.size()))
            break;

    } while (this->tryGettingNextPuzzleLocationToFill(threadData, currentPiece.location));

    currentPiece.location = stack.top();
    stack.pop();
    currentPiece.current = threadData.puzzleSolution[currentPiece.location.row][currentPiece.location.col];
    this->removePieceFromSolution(threadData, currentPiece.location);
    cout << "random thread " << threadData.id << " starting from random position at depth " << stack.size() << endl
         << std::flush;//TODO need to delete
    return true;
}

void PuzzleSolver::setRandomPiece(ThreadData &threadData, PuzzlePieceData &currentPiece) const {
    auto currentConstrain = threadData.puzzleConstrain[currentPiece.location.row][currentPiece.location.col];
    unsigned long numberOfPossiblePieces = 0;
    for (uint8_t piece = this->pieceManager->getNextPiece(threadData.pieceRepository, currentConstrain, nullPiece);
         piece != nullPiece; piece = this->pieceManager->getNextPiece(threadData.pieceRepository, currentConstrain,
                                                                      piece))
        numberOfPossiblePieces++;
    auto randomPieceIndex = static_cast<int>((rand() % numberOfPossiblePieces) + 1);
    currentPiece.current = nullPiece;
    for (int i = 0; i < randomPieceIndex; i++)
        currentPiece.current = this->pieceManager->getNextPiece(threadData.pieceRepository, currentConstrain,
                                                                currentPiece.current);
}

