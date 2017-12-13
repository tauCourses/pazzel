#include "PuzzleSolver.h"

bool SolverErrors::hasError() {
    return wrongNumberOfStraightLine || missingTL || missingTR || missingBL || missingBR || sumOfEdgesIsNotZero ||
           couldNotSolvePuzzle;
}

namespace {
    bool hasWrongNumberOfStraightLines(AbstractPieceManager &pm, int numberOfRows, int numberOfCols) {
        bool ret = false;
        ret |= pm.countConstrainPiece(hasLeftStraight) < numberOfRows;
        ret |= pm.countConstrainPiece(hasUpperStraight) < numberOfCols;
        ret |= pm.countConstrainPiece(hasRightStraight) < numberOfRows;
        ret |= pm.countConstrainPiece(hasDownStraight) < numberOfCols;
        return ret;
    }

    void hasWrongNumberOfStraightLines(SolverErrors &se, AbstractPieceManager &pm, int numberOfPieces) {
        bool wrongNumberOfStraightLines = true; // to be checked
        for (int i = 1; i * i <= numberOfPieces; ++i) {
            if (numberOfPieces % i == 0) {
                int row = i, col = numberOfPieces / i;
                // do one time if row == col else do 2 times row X col, col X row
                for (int j = 0; j < ((row == col) ? 1 : 2); ++j) {
                    wrongNumberOfStraightLines &= hasWrongNumberOfStraightLines(pm, row, col);

                    row = col, col = i;
                }
            }
        }
        se.wrongNumberOfStraightLine = wrongNumberOfStraightLines;
    }

    void hasASumOfZero(SolverErrors &se, AbstractPieceManager &pm) {
        se.sumOfEdgesIsNotZero =
                ((pm.countConstrainPiece(hasLeftMale) - pm.countConstrainPiece(hasRightFemale)) != 0) ||
                ((pm.countConstrainPiece(hasLeftFemale) - pm.countConstrainPiece(hasRightMale)) != 0) ||
                ((pm.countConstrainPiece(hasUpperMale) - pm.countConstrainPiece(hasDownFemale)) != 0) ||
                ((pm.countConstrainPiece(hasUpperFemale) - pm.countConstrainPiece(hasDownMale)) != 0);
    }

    void hasAllCorners(SolverErrors &se, AbstractPieceManager &pm) {
        se.missingTL = pm.countConstrainPiece(hasUpperStraight & hasLeftStraight) == 0;
        se.missingTR = pm.countConstrainPiece(hasUpperStraight & hasRightStraight) == 0;
        se.missingBL = pm.countConstrainPiece(hasDownStraight & hasLeftStraight) == 0;
        se.missingBR = pm.countConstrainPiece(hasDownStraight & hasRightStraight) == 0;
    }

}

PuzzleSolver::PuzzleSolver(const unique_ptr<AbstractPieceManager>& pieceManager) : pieceManager(pieceManager) {}

bool PuzzleSolver::trySolve() {
    auto allPossiblePuzzleShapes = pieceManager->getAllPossiblePuzzleShapes();
    for (auto const &shape:allPossiblePuzzleShapes)
        if (trySolveForShape(shape))
            return true;
    return false;
}

bool PuzzleSolver::trySolveForShape(AbstractPieceManager::Shape shape) {
    createNewPuzzleSolution(shape);
    std::deque<PuzzleLocation> puzzleLocationFilled;
    bool puzzleIsCompleted = false;
    // loop while puzzle is not completed or no piece to fill in.
    while (!puzzleIsCompleted) {
        //get next piece to fill
        PuzzleLocation puzzleLocation = getNextPuzzleLocationToFill();
        Piece_t currentPieceValue = 0;
        if (puzzleLocation.row < 0) { // no location to fill.
            if (puzzleLocationFilled.empty()) {
                // No solution possible.
                break;
            } else {
                puzzleLocation = puzzleLocationFilled.back(), puzzleLocationFilled.pop_back();
                currentPieceValue = puzzleSolution[puzzleLocation.row][puzzleLocation.col];
                removePieceFromSolution(puzzleLocation);
            }
        }
        Piece_t currentConstrain = puzzleConstrain[puzzleLocation.row][puzzleLocation.col];
        puzzleLocationFilled.push_back(puzzleLocation);
        currentPieceValue = pieceManager->getNextPiece(currentConstrain, currentPieceValue);
        updatePieceInSolution(puzzleLocation, currentPieceValue);
        puzzleIsCompleted = (shape.width * shape.height == puzzleLocationFilled.size());
    }
    return puzzleIsCompleted;
}

void PuzzleSolver::exportErrors(ofstream &out) {
    out << "Cannot solve puzzle: it seems that there is no proper solution" << "\n";
}

void PuzzleSolver::exportSolution(ofstream &out) {
    // todo
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
    if (puzzleLocation.row < puzzleConstrain.size() - 1) {
        puzzleConstrain[puzzleLocation.row + 1][puzzleLocation.col] |= 0x3 << 4;
    }
    if (puzzleLocation.col < puzzleConstrain[0].size() - 1) {
        puzzleConstrain[puzzleLocation.row][puzzleLocation.col + 1] |= 0x3 << 6;
    }
}

void PuzzleSolver::updatePieceInSolution(PuzzleSolver::PuzzleLocation puzzleLocation, Piece_t currentPiece) {
    puzzleSolution[puzzleLocation.row][puzzleLocation.col] = currentPiece;
    if (puzzleLocation.row > 0) {
        puzzleConstrain[(puzzleLocation.row - 1)][puzzleLocation.col] &=
                (getConstrainOpposite(static_cast<uint8_t>((currentPiece >> 4) & 0x3)) << 0) | ((0x3 << 0) ^ nullPiece);
    }
    if (puzzleLocation.col > 0) {
        puzzleConstrain[puzzleLocation.row][puzzleLocation.col - 1] &=
                (getConstrainOpposite(static_cast<uint8_t>((currentPiece >> 6) & 0x3)) << 2) | ((0x3 << 2) ^ nullPiece);
    }
    if (puzzleLocation.row < puzzleConstrain.size() - 1) {
        puzzleConstrain[(puzzleLocation.row + 1)][puzzleLocation.col] &=
                (getConstrainOpposite(static_cast<uint8_t>((currentPiece >> 0) & 0x3)) << 4) | ((0x3 << 4) ^ nullPiece);
    }
    if (puzzleLocation.col < puzzleConstrain[0].size() - 1) {
        puzzleConstrain[puzzleLocation.row][puzzleLocation.col + 1] &=
                (getConstrainOpposite(static_cast<uint8_t>((currentPiece >> 2) & 0x3)) << 6) | ((0x3 << 6) ^ nullPiece);
    }
}

PuzzleSolver::PuzzleLocation PuzzleSolver::getNextPuzzleLocationToFill() {
   /* int minOption = INT_MAX;
    PuzzleLocation bestLocation = {.col = -1, .row = -1}, currentLocation = {.row = 0, .col = 0};
    int row = (int)puzzleSolution.size(), col = (int)puzzleSolution[0].size();
    for (currentLocation.row = 0; currentLocation.row < row; ++currentLocation.row) {
        for (currentLocation.col = 0; currentLocation.col < col; ++currentLocation.col) {
            if (nullPiece == puzzleSolution[currentLocation.row][currentLocation.col]) {
                int currentConstrainCount = pieceManager->countConstrainOptions(
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
    return bestLocation;*/
}

inline Piece_t PuzzleSolver::getConstrainOpposite(Piece_t currentConstrain) {
    return static_cast<uint8_t>(
            (((currentConstrain & 0x1) == (uint8_t) (currentConstrain >> 1)) << 1) |
            (currentConstrain & 0x1));
}