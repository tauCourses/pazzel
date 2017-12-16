#include "BasicPieceManager.h"

void BasicPieceManager::addPiece(unique_ptr<PuzzlePiece> piece) {
    this->pieces.emplace_back(piece.get());
    addPieceToRepository(piece->representor());
    for (Piece_t maskOption : maskOptions) {
        if (maskOption != 0)
            ++constrainRepository[piece->representor() | maskOption];
    }
}

inline int BasicPieceManager::numOfOptionsForConstrain(Piece_t constrain) {
    return constrainOption[constrain];
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
    return constrainRepository[hasLeftStraight] >= shape.height &&
           constrainRepository[hasUpperStraight] >= shape.width &&
           constrainRepository[hasRightStraight] >= shape.height &&
           constrainRepository[hasDownStraight] >= shape.width;
}

bool BasicPieceManager::hasASumOfZero() {
    return constrainRepository[hasLeftMale] - constrainRepository[hasRightFemale] == 0 &&
           constrainRepository[hasLeftFemale] - constrainRepository[hasRightMale] == 0 &&
           constrainRepository[hasUpperMale] - constrainRepository[hasDownFemale] == 0 &&
           constrainRepository[hasUpperFemale] - constrainRepository[hasDownMale] == 0;
}

bool BasicPieceManager::hasAllCorners() {
    return constrainRepository[hasUpperStraight & hasLeftStraight] != 0 &&
           constrainRepository[hasUpperStraight & hasRightStraight] != 0 &&
           constrainRepository[hasDownStraight & hasLeftStraight] != 0 &&
           constrainRepository[hasDownStraight & hasRightStraight] != 0;
}

void BasicPieceManager::printMissingCorners(ofstream &fout) {
    const char *message = "Cannot solve puzzle: missing corner element: ";
    if (constrainRepository[hasUpperStraight & hasLeftStraight] == 0) { fout << message << "TL" << endl; }
    if (constrainRepository[hasUpperStraight & hasRightStraight] == 0) { fout << message << "TR" << endl; }
    if (constrainRepository[hasDownStraight & hasLeftStraight] == 0) { fout << message << "BL" << endl; }
    if (constrainRepository[hasDownStraight & hasRightStraight] == 0) { fout << message << "BR" << endl; }
}

void BasicPieceManager::addPieceToRepository(Piece_t piece) {
    if (constrainRepository[piece]++ == 0) {
        addPieceToOption(piece);
    }
}

void BasicPieceManager::removePieceFromRepository(Piece_t piece) {
    if (--constrainRepository[piece] == 0) {
        removePieceFromOption(piece);
    }
}

void BasicPieceManager::addPieceToOption(Piece_t piece) {
    for (Piece_t maskOption : maskOptions) {
        ++constrainOption[piece | maskOption];
    }
}

void BasicPieceManager::removePieceFromOption(Piece_t piece) {
    for (Piece_t maskOption : maskOptions) {
        --constrainOption[piece | maskOption];
    }
}

void BasicPieceManager::printPiece(Piece_t piece, ofstream &out) {
    for (auto it = pieces.begin(); it != pieces.end(); ++it) {
        if (it->representor() == piece && !it->wasUsedInSolution) {
            it->wasUsedInSolution = true;
            out << it->index;
            return;
        }
    }
    cerr << "Error: couldn't find piece: " << int(piece) << endl;
}

inline bool BasicPieceManager::pieceExistInRepository(Piece_t piece) {
    return this->constrainRepository[piece] > 0;
}
