#include "PuzzleSolver.h"

PuzzleSolver::PuzzleSolver(const unique_ptr<AbstractPieceManager> &pieceManager, int numberOfThreads)
        : pieceManager(pieceManager), numberOfThreads(numberOfThreads),
          numberOfPieces(pieceManager->getNumberOfPieces()) {}

void PuzzleSolver::solve() {
    this->pieceManager->preConditions(); //check pre-conditions
        throw PuzzleException(NO_PROPER_SOLUTION);

    this->initPuzzleShapesVectors();

    vector<std::thread> randomThreadsVector;
    for (int i = 0; i < this->numberOfThreads - 1; ++i)  //one thread is the main thread
        randomThreadsVector.emplace_back(std::thread(&PuzzleSolver::runThread, this, ThreadType::RANDOM));

    runThread(ThreadType::SERIAL);
    this->sharedData.sharedDataMutex.lock();
    this->sharedData.terminateThreads = true;
    this->sharedData.sharedDataMutex.unlock();
    for (auto &threadRun : randomThreadsVector)
        if (threadRun.joinable())
            threadRun.join();

    if(!this->sharedData.solutionFound)
        throw PuzzleException(NO_PROPER_SOLUTION);
}

void PuzzleSolver::initPuzzleShapesVectors() {
    this->allPossiblePuzzleShapes = this->pieceManager->getAllPossiblePuzzleShapes();
    this->shapesForSerialScanning = this->allPossiblePuzzleShapes;
    auto rng = std::default_random_engine {};
    rng.seed(unsigned(time(nullptr)));
    std::shuffle(std::begin(this->shapesForSerialScanning), std::end(this->shapesForSerialScanning), rng);
}

void PuzzleSolver::runThread(ThreadType type) {
    this->puzzleSolverMutex.lock();
    SolverThread solverThread(this->sharedData, this->pieceManager, type);
    this->puzzleSolverMutex.unlock();

    while (tryInitNextThreadRun(solverThread)) {
        if (solverThread.trySolve()) {
            this->puzzleSolverMutex.lock();
            solverThread.exportThreadSolution(this->puzzleSolution);
            this->puzzleSolverMutex.unlock();
            break;
        }
    }
}

bool PuzzleSolver::tryInitNextThreadRun(SolverThread& solverThread){
    this->sharedData.sharedDataMutex.lock();
    if (this->sharedData.terminateThreads) {
        this->sharedData.sharedDataMutex.unlock();
        return false;
    }
    this->sharedData.sharedDataMutex.unlock();
    if (solverThread.type == ThreadType::RANDOM)
        this->initRandomizeRun(solverThread);
    else
        return tryInitSerialRun(solverThread);

    return true;
}

bool PuzzleSolver::tryInitSerialRun(SolverThread& solverThread) {//globalDataMutex has to be locked!
    this->puzzleSolverMutex.lock();
    if(this->shapesForSerialScanning.empty())
    {
        this->puzzleSolverMutex.unlock();
        return false;
    }
    solverThread.shape = this->shapesForSerialScanning.back();
    this->shapesForSerialScanning.pop_back();
    this->puzzleSolverMutex.unlock();
    return true;
}

void PuzzleSolver::initRandomizeRun(SolverThread& solverThread) {//global Data Mutex has to be locked!
    unsigned long random_integer = (rand() % this->allPossiblePuzzleShapes.size());
    this->puzzleSolverMutex.lock();
    solverThread.shape = this->allPossiblePuzzleShapes.at(random_integer);
    solverThread.retrievePieceManager(this->pieceManager);
    this->puzzleSolverMutex.unlock();
}

void PuzzleSolver::exportSolution(ofstream &out) const {
    auto height = puzzleSolution.size();
    auto width = puzzleSolution[0].size();
    for (unsigned int i = 0; i < height; i++) {
        for (unsigned int j = 0; j < width; j++) {
            this->pieceManager->printPiece(puzzleSolution[i][j], out);
            if (j + 1 != width)
                out << " ";
            else
                out << endl;
        }
    }
}