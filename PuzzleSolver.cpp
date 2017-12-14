#include "PuzzleSolver.h"

PuzzleSolver::PuzzleSolver(const unique_ptr<AbstractPieceManager> &pieceManager) : pieceManager(pieceManager) {}

bool PuzzleSolver::trySolve() {
    auto allPossiblePuzzleShapes = pieceManager->getAllPossiblePuzzleShapes();
    for (auto const &shape:allPossiblePuzzleShapes)
        if (trySolveForShape(shape))
            return true;
    return false;
}

bool PuzzleSolver::trySolveForShape(AbstractPieceManager::Shape shape) {
    createNewPuzzleSolution(shape);
    int numberOfPieces = shape.width * shape.height;
    vector<PuzzleLocation> puzzleLocationFilled(numberOfPieces);
    int puzzleLocationFilledSize = 0;
    bool puzzleIsCompleted = false;
    // loop while puzzle is not completed or no piece to fill in.
    while (!puzzleIsCompleted) {
        //get next piece to fill
        PuzzleLocation puzzleLocation = getNextPuzzleLocationToFill();
        Piece_t currentPieceValue = nullPiece;
        if (puzzleLocation.row < 0) { // no location to fill.
            if (puzzleLocationFilledSize == 0) {
                // No solution possible.
                break;
            } else {
                puzzleLocation = puzzleLocationFilled[--puzzleLocationFilledSize];
                currentPieceValue = puzzleSolution[puzzleLocation.row][puzzleLocation.col];
                removePieceFromSolution(puzzleLocation);
            }
        }
        Piece_t currentConstrain = puzzleConstrain[puzzleLocation.row][puzzleLocation.col];
        puzzleLocationFilled[puzzleLocationFilledSize++] = puzzleLocation;
        currentPieceValue = pieceManager->getNextPiece(currentConstrain, currentPieceValue);
        updatePieceInSolution(puzzleLocation, currentPieceValue);
        puzzleIsCompleted = numberOfPieces == puzzleLocationFilledSize;
    }
    return puzzleIsCompleted;
}

void PuzzleSolver::exportErrors(ofstream &out) {
    out << "Cannot solve puzzle: it seems that there is no proper solution" << endl;
}

void PuzzleSolver::exportSolution(ofstream &out) {
    auto height = static_cast<int>(puzzleSolution.size()),
            width = static_cast<int>(puzzleSolution[0].size());
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

void PuzzleSolver::createNewPuzzleSolution(AbstractPieceManager::Shape shape) {
    puzzleSolution.clear();
    puzzleConstrain.clear();

    for (int i = 0; i < shape.height; i++) {
        puzzleSolution.emplace_back(shape.width, nullPiece);
        puzzleConstrain.emplace_back(shape.width, nullPiece);
    }
    for (int i = 0; i < shape.width; i++) {
        puzzleConstrain[0][i] &= hasUpperStraight;
        puzzleConstrain[shape.height - 1][i] &= hasDownStraight;
    }
    for (int i = 0; i < shape.height; i++) {
        puzzleConstrain[i][0] &= hasLeftStraight;
        puzzleConstrain[i][shape.width - 1] &= hasRightStraight;
    }

}

void PuzzleSolver::removePieceFromSolution(PuzzleSolver::PuzzleLocation puzzleLocation) {
    puzzleSolution[puzzleLocation.row][puzzleLocation.col] = nullPiece;
    if (puzzleLocation.row > 0) {
        puzzleConstrain[puzzleLocation.row - 1][puzzleLocation.col] |= 0x3 << 0;
    }
    if (puzzleLocation.col > 0) {
        puzzleConstrain[puzzleLocation.row][puzzleLocation.col - 1] |= 0x3 << 2;
    }
    if (puzzleLocation.row < static_cast<int>(puzzleConstrain.size()) - 1) {
        puzzleConstrain[puzzleLocation.row + 1][puzzleLocation.col] |= 0x3 << 4;
    }
    if (puzzleLocation.col < static_cast<int>(puzzleConstrain[0].size()) - 1) {
        puzzleConstrain[puzzleLocation.row][puzzleLocation.col + 1] |= 0x3 << 6;
    }
}

void PuzzleSolver::updatePieceInSolution(PuzzleSolver::PuzzleLocation puzzleLocation, Piece_t currentPiece) {
    puzzleSolution[puzzleLocation.row][puzzleLocation.col] = currentPiece;
    if (puzzleLocation.row > 0) {
        puzzleConstrain[(puzzleLocation.row - 1)][puzzleLocation.col] &=
                (getConstrainOpposite(static_cast<Piece_t>((currentPiece >> 4) & 0x3)) << 0) | ((0x3 << 0) ^ nullPiece);
    }
    if (puzzleLocation.col > 0) {
        puzzleConstrain[puzzleLocation.row][puzzleLocation.col - 1] &=
                (getConstrainOpposite(static_cast<Piece_t>((currentPiece >> 6) & 0x3)) << 2) | ((0x3 << 2) ^ nullPiece);
    }
    if (puzzleLocation.row < static_cast<int>(puzzleConstrain.size()) - 1) {
        puzzleConstrain[(puzzleLocation.row + 1)][puzzleLocation.col] &=
                (getConstrainOpposite(static_cast<Piece_t>((currentPiece >> 0) & 0x3)) << 4) | ((0x3 << 4) ^ nullPiece);
    }
    if (puzzleLocation.col < static_cast<int>(puzzleConstrain[0].size()) - 1) {
        puzzleConstrain[puzzleLocation.row][puzzleLocation.col + 1] &=
                (getConstrainOpposite(static_cast<Piece_t>((currentPiece >> 2) & 0x3)) << 6) | ((0x3 << 6) ^ nullPiece);
    }
}

PuzzleSolver::PuzzleLocation PuzzleSolver::getNextPuzzleLocationToFill() {
    int minOption = INT_MAX;
    PuzzleLocation bestLocation = {-1, -1}, currentLocation = {-1, -1};
    auto row = static_cast<int>(puzzleSolution.size()), col = static_cast<int>(puzzleSolution[0].size());
    for (currentLocation.row = 0; currentLocation.row < row; ++currentLocation.row) {
        for (currentLocation.col = 0; currentLocation.col < col; ++currentLocation.col) {
            if (nullPiece == puzzleSolution[currentLocation.row][currentLocation.col]) {
                int currentConstrainCount = pieceManager->numOfOptionsForConstrain(
                        puzzleConstrain[currentLocation.row][currentLocation.col]);
                if (currentConstrainCount < minOption) {
                    if (currentConstrainCount == 0) {
                        return bestLocation; // no available piece for a location, should go back.
                    } else {
                        minOption = currentConstrainCount;
                        bestLocation = currentLocation;
                    }
                }
            }
        }
    }
    return bestLocation;
}

inline Piece_t PuzzleSolver::getConstrainOpposite(Piece_t currentConstrain) {
    return static_cast<uint8_t>(
            (((currentConstrain & 0x1) == (uint8_t) (currentConstrain >> 1)) << 1) |
            (currentConstrain & 0x1));
}