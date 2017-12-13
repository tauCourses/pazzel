#include "BasicPieceManager.h"

BasicPieceManager::BasicPieceManager() {
    constrainCount.resize(numberOfConstrains);
    constrainOption.resize(numberOfConstrains);
    createMaskVector();
    createNextPieceVector();
}

void BasicPieceManager::addPiece(unique_ptr<PuzzlePiece> piece) {
    this->pieces.emplace_back(piece.get());
    addPieceToCount(piece.get().representor());
}

inline int BasicPieceManager::countConstrainOptions(Piece_t constrain) {
    return constrainOption[constrain] * constrainOption[nullPiece] +
           constrainCount[constrain]; // todo delete this if it's slower this way. it's a change in the algorithm
}

inline int BasicPieceManager::countConstrainPiece(Piece_t constrain) {
    return constrainCount[constrain];
}

Piece_t BasicPieceManager::getNextPiece(Piece_t constrain, Piece_t last) {
    return 0;
}

vector<AbstractPieceManager::Shape> BasicPieceManager::getAllPossiblePuzzleShapes() {
    vector<AbstractPieceManager::Shape> possibleShapes;
    Shape shape;
    shape.width = shape.height = 1;
    auto numberOfPieces = static_cast<int>(pieces.size());
    for (; shape.width <= numberOfPieces; shape.width++) {
        shape.height = numberOfPieces / shape.width;
        if (numberOfPieces == shape.height * shape.width && isPuzzleShapePossible(shape)) {
            possibleShapes.push_back(shape);
        }
    }
    return possibleShapes;
}

bool BasicPieceManager::isPuzzleShapePossible(AbstractPieceManager::Shape shape) {
    return countConstrainPiece(hasLeftStraight) >= shape.height &&
           countConstrainPiece(hasUpperStraight) >= shape.width &&
           countConstrainPiece(hasRightStraight) >= shape.height &&
           countConstrainPiece(hasDownStraight) >= shape.width;
}

bool BasicPieceManager::hasASumOfZero() {
    return countConstrainPiece(hasLeftMale) - countConstrainPiece(hasRightFemale) == 0 &&
           countConstrainPiece(hasLeftFemale) - countConstrainPiece(hasRightMale) == 0 &&
           countConstrainPiece(hasUpperMale) - countConstrainPiece(hasDownFemale) == 0 &&
           countConstrainPiece(hasUpperFemale) - countConstrainPiece(hasDownMale) == 0;
}

bool BasicPieceManager::hasAllCorners() {
    return countConstrainPiece(hasUpperStraight & hasLeftStraight) != 0 &&
           countConstrainPiece(hasUpperStraight & hasRightStraight) != 0 &&
           countConstrainPiece(hasDownStraight & hasLeftStraight) != 0 &&
           countConstrainPiece(hasDownStraight & hasRightStraight) != 0;
}

void BasicPieceManager::printMissingCorners(ofstream &fout) {
    const char *message = "Cannot solve puzzle: missing corner element: ";
    if (countConstrainPiece(hasUpperStraight & hasLeftStraight) == 0) { fout << message << "TL" << endl; }
    if (countConstrainPiece(hasUpperStraight & hasRightStraight) == 0) { fout << message << "TR" << endl; }
    if (countConstrainPiece(hasDownStraight & hasLeftStraight) == 0) { fout << message << "BL" << endl; }
    if (countConstrainPiece(hasDownStraight & hasRightStraight) == 0) { fout << message << "BR" << endl; }
}


void BasicPieceManager::createMaskVector() {
    maskVector.resize(1 << 4);
    Piece_t maskPiece;
    for (int maskValue = 0; maskValue < (1 << 4); ++maskValue) {
        maskPiece = 0;
        if ((maskValue & (1 << 3)) == 0) maskPiece |= (0x3 << 6);
        if ((maskValue & (1 << 2)) == 0) maskPiece |= (0x3 << 4);
        if ((maskValue & (1 << 1)) == 0) maskPiece |= (0x3 << 2);
        if ((maskValue & (1 << 0)) == 0) maskPiece |= (0x3 << 0);
        maskVector[maskValue] = maskPiece;
    }
}

void BasicPieceManager::createNextPieceVector() {
    for (int i = 0; i < numberOfConstrains; i++) nextPieceVector.emplace_back(numberOfConstrains);
    for (int currentConstrain = 0; currentConstrain < numberOfConstrains; currentConstrain++) {
        for (int currentPiece = 0; currentPiece < numberOfConstrains; currentPiece++) {
            int nextPiece = (currentPiece != nullPiece) ? currentPiece + 1 : currentPiece = 0;
            while ((nextPiece & currentConstrain) != currentPiece
                   && ((currentPiece & (3 << 0)) == (3 << 0) || (currentPiece & (3 << 2)) == (3 << 2) ||
                       (currentPiece & (3 << 4)) == (3 << 4) || (currentPiece & (3 << 6)) == (3 << 6))
                   && nextPiece != nullPiece) {
                currentPiece++;
            }
        }
    }
}

void BasicPieceManager::addPieceToCount(Piece_t piece) {
    if (constrainCount[piece] == 0) {
        addPieceToOptions(piece);
    }
    for (Piece_t maskOption : maskVector) {
        ++constrainCount[piece | maskOption];
    }
}

void BasicPieceManager::removePieceFromCount(Piece_t piece) {
    for (Piece_t maskOption : maskVector) {
        --constrainCount[piece | maskOption];
    }
    if (constrainCount[piece] == 0) {
        removePieceFromOptions(piece);
    }
}

void BasicPieceManager::addPieceToOptions(Piece_t piece) {
    for (Piece_t maskOption : maskVector) {
        ++constrainOption[piece | maskOption];
    }
}

void BasicPieceManager::removePieceFromOptions(Piece_t piece) {
    for (Piece_t maskOption : maskVector) {
        --constrainOption[piece | maskOption];
    }
}

unique_ptr<PuzzlePiece> BasicPieceManager::getPieceOfType(Piece_t piece) {
    for (auto it = pieces.begin(); it != pieces.end(); ++it) {
        if (it->representor() == piece) {
            unique_ptr<PuzzlePiece> puzzlePiece = std::make_unique<PuzzlePiece>(*it);
            pieces.erase(it);
            return puzzlePiece;
        }
    }
}

