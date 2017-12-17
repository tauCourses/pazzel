#include "BasicPieceManager.h"

void BasicPieceManager::addPiece(unique_ptr<PuzzlePiece> piece) {
    this->pieces.emplace_back(piece.get());
    this->addPieceToRepository(piece->representor());
    for (Piece_t maskOption : this->maskOptions) {
        if (maskOption != 0)
            ++this->constrainRepository[piece->representor() | maskOption];
    }
}

inline int BasicPieceManager::numOfOptionsForConstrain(Piece_t constrain) {
    return this->constrainOption[constrain];
}


vector<AbstractPieceManager::Shape> BasicPieceManager::getAllPossiblePuzzleShapes() {
    vector<AbstractPieceManager::Shape> possibleShapes;
    Shape shape;
    shape.width = shape.height = 1;
    auto numberOfPieces = static_cast<int>(this->pieces.size());
    for (; shape.width <= numberOfPieces; shape.width++) {
        shape.height = numberOfPieces / shape.width;
        if (numberOfPieces == shape.height * shape.width && this->isPuzzleShapePossible(shape)) {
            possibleShapes.push_back(shape);
        }
    }
    return possibleShapes;
}

bool BasicPieceManager::isPuzzleShapePossible(AbstractPieceManager::Shape shape) {
    return this->constrainRepository[hasLeftStraight] >= shape.height &&
           this->constrainRepository[hasUpperStraight] >= shape.width &&
           this->constrainRepository[hasRightStraight] >= shape.height &&
           this->constrainRepository[hasDownStraight] >= shape.width;
}

bool BasicPieceManager::preConditions() {
    return this->constrainRepository[hasLeftMale] - this->constrainRepository[hasRightFemale] == 0 &&
           this->constrainRepository[hasLeftFemale] - this->constrainRepository[hasRightMale] == 0 &&
           this->constrainRepository[hasUpperMale] - this->constrainRepository[hasDownFemale] == 0 &&
           this->constrainRepository[hasUpperFemale] - this->constrainRepository[hasDownMale] == 0;
}

bool BasicPieceManager::hasAllCorners() {
    return this->constrainRepository[hasUpperStraight & hasLeftStraight] != 0 &&
           this->constrainRepository[hasUpperStraight & hasRightStraight] != 0 &&
           this->constrainRepository[hasDownStraight & hasLeftStraight] != 0 &&
           this->constrainRepository[hasDownStraight & hasRightStraight] != 0;
}

void BasicPieceManager::printMissingCorners(ofstream &fout) {
    if(hasAllCorners())
        return;
    const char *message = "Cannot solve puzzle: missing corner element: ";
    if (this->constrainRepository[hasUpperStraight & hasLeftStraight] == 0)
        fout << message << "TL" << endl;
    if (this->constrainRepository[hasUpperStraight & hasRightStraight] == 0)
        fout << message << "TR" << endl;
    if (this->constrainRepository[hasDownStraight & hasLeftStraight] == 0)
        fout << message << "BL" << endl;
    if (this->constrainRepository[hasDownStraight & hasRightStraight] == 0)
        fout << message << "BR" << endl;
}

void BasicPieceManager::addPieceToRepository(Piece_t piece) {
    if (this->constrainRepository[piece]++ == 0)
        this->addPieceToOption(piece);
}

void BasicPieceManager::removePieceFromRepository(Piece_t piece) {
    if (--this->constrainRepository[piece] == 0)
        this->removePieceFromOption(piece);
}

void BasicPieceManager::addPieceToOption(Piece_t piece) {
    for (Piece_t maskOption : this->maskOptions)
        ++this->constrainOption[piece | maskOption];
}

void BasicPieceManager::removePieceFromOption(Piece_t piece) {
    for (Piece_t maskOption : this->maskOptions)
        --this->constrainOption[piece | maskOption];
}

void BasicPieceManager::printPiece(Piece_t piece, ofstream &out) {
    for (auto &it : this->pieces) {
        if (it.representor() == piece && !it.wasUsedInSolution) {
            it.wasUsedInSolution = true;
            out << it.index;
            return;
        }
    }
    cerr << "Error: couldn't find piece: " << int(piece) << endl;
}

inline bool BasicPieceManager::pieceExistInRepository(Piece_t piece) {
    return this->constrainRepository[piece] > 0;
}
