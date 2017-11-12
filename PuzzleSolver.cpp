//
// Created by t-idkess on 11/11/2017.
//

#include "PuzzleSolver.h"

namespace {
    bool hasWrongNumberOfStraightLine(PieceManager &pieceManager, int numberOfRows, int numberOfCols) {
        bool ret = false;
        ret |= pieceManager.countConstrainPiece(0b10000000u) < numberOfRows;
        ret |= pieceManager.countConstrainPiece(0b00100000u) < numberOfCols;
        ret |= pieceManager.countConstrainPiece(0b00001000u) < numberOfRows;
        ret |= pieceManager.countConstrainPiece(0b00000010u) < numberOfCols;
        return ret;
    }
}

PuzzleSolver::PuzzleSolver(ParsedPuzzle &puzzle) : pieceManager(puzzle) {
    solverErrors.missingTL = pieceManager.countConstrainPiece(0b10100000u) == 0;
    solverErrors.missingTR = pieceManager.countConstrainPiece(0b00101000u) == 0;
    solverErrors.missingBL = pieceManager.countConstrainPiece(0b10000010u) == 0;
    solverErrors.missingBR = pieceManager.countConstrainPiece(0b00001010u) == 0;
    solverErrors.sumOfEdgesIsNotZero =
            ((pieceManager.countConstrainPiece(0b01000000u) - pieceManager.countConstrainPiece(0b00001100u)) != 0) ||
            ((pieceManager.countConstrainPiece(0b11000000u) - pieceManager.countConstrainPiece(0b00000100u)) != 0) ||
            ((pieceManager.countConstrainPiece(0b00010000u) - pieceManager.countConstrainPiece(0b00000011u)) != 0) ||
            ((pieceManager.countConstrainPiece(0b00110000u) - pieceManager.countConstrainPiece(0b00000001u)) != 0);
    bool wrongNumberOfStraightLine = true;
    for (int i = 1; i * i <= puzzle.numberOfPieces; ++i) {
        if (puzzle.numberOfPieces % i == 0) {
            int row = i, col = puzzle.numberOfPieces / i;
            // do one time if row == col else do 2 times row X col, col X row
            for (int j = 0; j < ((row == col) ? 1 : 2); ++j) {
                bool hasWrongNumberOfStraightLines = hasWrongNumberOfStraightLine(pieceManager, row, col);
                if (!solverErrors.hasError() && !hasWrongNumberOfStraightLines &&
                    solvePuzzle(row, col)) {
                    return;
                }
                wrongNumberOfStraightLine &= hasWrongNumberOfStraightLines;

                row = col;
                col = i;
            }
        }
    }
    solverErrors.wrongNumberOfStraightLine = wrongNumberOfStraightLine;
    if (!solverErrors.hasError()) solverErrors.couldNotSolvePuzzle = true;
}

PuzzleSolver::~PuzzleSolver() {
    if (puzzleSolution != nullptr)delete puzzleSolution;
}


bool SolverErrors::hasError() {
    return wrongNumberOfStraightLine || missingTL || missingTR || missingBL || missingBR || sumOfEdgesIsNotZero ||
           couldNotSolvePuzzle;
}

PuzzleSolution::PuzzleSolution(int row, int col) : col(col), row(row) {
    puzzleSolution = new Piece_t[row * col]();
}

PuzzleSolution::~PuzzleSolution() {
    if (puzzleSolution != nullptr)delete[] puzzleSolution;
}


Piece_t inline PuzzleSolution::get(int _row, int _col) {
    return puzzleSolution[_row * col + _col];
}

void inline PuzzleSolution::set(int _row, int _col, Piece_t newVal) {
    puzzleSolution[_row * col + _col] = newVal;
}

namespace {
    typedef struct {
        int rowNumber, colNumber;
    } PuzzlePieceLocation;

    PuzzlePieceLocation nextLocationToCheck(PuzzlePieceLocation *queue, int queueSize, PuzzleSolution *puzzleSolution) {
        //SHABLUL! todo
        int i = 0;
        PuzzlePieceLocation ret = {0, 0};
        int shublulNumber = 0;
        while (i < queueSize) {
            while (ret.colNumber < puzzleSolution->col - 1 - shublulNumber && i < queueSize) {
                ++ret.colNumber;
                ++i;
            }
            while (ret.rowNumber < puzzleSolution->row - 1 - shublulNumber && i < queueSize) {
                ++ret.rowNumber;
                ++i;
            }
            while (ret.colNumber > shublulNumber && i < queueSize) {
                --ret.colNumber;
                ++i;
            }
            while (ret.rowNumber > shublulNumber + 1 && i < queueSize) {
                --ret.rowNumber;
                ++i;
            }
            ++shublulNumber;
        }
        return ret;
    }

    inline Piece_t getConstrainOposite(Piece_t currentConstrain) {
        switch (currentConstrain) {
            case 0b01:
                currentConstrain = 0b11;
                break;
            case 0b11:
                currentConstrain = 0b01;
                break;
            default:
                break;
        }
        return currentConstrain;
    }

    Piece_t getLocationConstraints(PuzzleSolution *puzzleSolution, const PuzzlePieceLocation &currentLocation) {
        Piece_t constraints = nullPiece;
        Piece_t currentPiece;
        if (currentLocation.colNumber > 0) {
            currentPiece = puzzleSolution->get(currentLocation.rowNumber, currentLocation.colNumber - 1);
            currentPiece = (currentPiece >> 2);
            currentPiece &= 0b11u;
            constraints |= getConstrainOposite(currentPiece) << 6;
        } else {
            constraints |= 0b10u << 6;
        }
        if (currentLocation.rowNumber > 0) {
            currentPiece = puzzleSolution->get(currentLocation.rowNumber - 1, currentLocation.colNumber);
            currentPiece = (currentPiece >> 0);
            currentPiece &= 0b11u;
            constraints |= getConstrainOposite(currentPiece) << 4;
        } else {
            constraints |= 0b10u << 4;
        }
        if (currentLocation.colNumber + 1 < puzzleSolution->col) {
            currentPiece = puzzleSolution->get(currentLocation.rowNumber, currentLocation.colNumber + 1);
            currentPiece = (currentPiece >> 6);
            currentPiece &= 0b11u;
            constraints |= getConstrainOposite(currentPiece) << 2;
        } else {
            constraints |= 0b10u << 2;
        }
        if (currentLocation.rowNumber + 1 < puzzleSolution->row) {
            currentPiece = puzzleSolution->get(currentLocation.rowNumber + 1, currentLocation.colNumber);
            currentPiece = (currentPiece >> 4);
            currentPiece &= 0b11u;
            constraints |= getConstrainOposite(currentPiece) << 0;
        } else {
            constraints |= 0b10u << 0;
        }
        return constraints;
    }
}

bool PuzzleSolver::solvePuzzle(int row, int col) {
    int queueLength = row * col;
    auto *queue = new PuzzlePieceLocation[queueLength];
    int queueCurrentSize = 0;
    if (puzzleSolution != nullptr) delete puzzleSolution;
    puzzleSolution = new PuzzleSolution(row, col);
    bool shouldCheckNewLocation = true;
    PuzzlePieceLocation pieceLocation;
    do {
        Piece_t lastValue = nullPiece;
        if (shouldCheckNewLocation) {
            pieceLocation = nextLocationToCheck(queue, queueCurrentSize, puzzleSolution);
        } else {
            pieceLocation = queue[--queueCurrentSize];
            lastValue = puzzleSolution->get(pieceLocation.rowNumber, pieceLocation.colNumber);
        }
        Piece_t constraints = getLocationConstraints(puzzleSolution, pieceLocation);
        Piece_t newValue = pieceManager.getNextPiece(constraints, lastValue);
        puzzleSolution->set(pieceLocation.rowNumber, pieceLocation.colNumber, newValue);

        if (newValue == nullPiece) {
            shouldCheckNewLocation = false;
        } else {
            shouldCheckNewLocation = true;
            queue[queueCurrentSize++] = pieceLocation;
        }
    } while (!(!shouldCheckNewLocation && queueCurrentSize == 0) && queueCurrentSize < queueLength);

    if (queue != nullptr) delete[] queue;
    return queueCurrentSize == queueLength;
}