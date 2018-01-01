#include "BasicPieceManager.h"


void BasicPieceManager::addPiece(unique_ptr<PuzzlePiece> piece) {
    this->pieces.emplace_back(make_tuple(PuzzlePiece(piece.get()),false));
    this->addPieceToRepository(piece->representor());
}

inline int BasicPieceManager::numOfOptionsForConstrain(Piece_t constrain) const {
    return constrainRepository[constrain];
}

vector<AbstractPieceManager::Shape> BasicPieceManager::getAllPossiblePuzzleShapes() const {
    vector<AbstractPieceManager::Shape> possibleShapes;
    auto numberOfPieces = static_cast<int>(this->pieces.size());
    AbstractPieceManager::Shape shape = {1, 1};
    for (; shape.width <= numberOfPieces; shape.width++) {
        shape.height = numberOfPieces / shape.width;
        if (numberOfPieces == shape.height * shape.width && this->isPuzzleShapePossible(shape)) {
            possibleShapes.push_back(shape);
        }
    }
    return possibleShapes;
}

bool BasicPieceManager::isPuzzleShapePossible(AbstractPieceManager::Shape shape) const {
    int leftStraightCount = 0, rightStraightCount = 0, upStraightCount = 0, downStraightCount = 0;
    for (auto &pieceTuple : this->pieces) {
        auto piece = get<0>(pieceTuple);
        if (piece.left == 0) leftStraightCount++;
        if (piece.right == 0) rightStraightCount++;
        if (piece.up == 0) upStraightCount++;
        if (piece.down == 0) downStraightCount++;
    }
    return leftStraightCount >= shape.height &&
           rightStraightCount >= shape.height &&
           upStraightCount >= shape.width &&
           downStraightCount >= shape.width;
}

bool BasicPieceManager::preConditions() const {
    int leftMaleRightFemaleRatio = 0, leftFemaleRightMaleRatio = 0, upMaleDownFemaleRatio = 0, upFemaleDownMaleRatio = 0;
    for (auto &pieceTuple : this->pieces) {
        auto piece = get<0>(pieceTuple);
        if (piece.left == 1) leftMaleRightFemaleRatio++;
        if (piece.right == -1) leftMaleRightFemaleRatio--;
        if (piece.left == -1) leftFemaleRightMaleRatio++;
        if (piece.right == 1) leftFemaleRightMaleRatio--;
        if (piece.up == 1) upMaleDownFemaleRatio++;
        if (piece.down == -1) upMaleDownFemaleRatio--;
        if (piece.up == -1) upFemaleDownMaleRatio++;
        if (piece.down == 1) upFemaleDownMaleRatio--;
    }
    return leftMaleRightFemaleRatio == 0 &&
           leftFemaleRightMaleRatio == 0 &&
           upMaleDownFemaleRatio == 0 &&
           upFemaleDownMaleRatio == 0;
}

bool BasicPieceManager::hasAllCorners() const {
    return this->constrainRepository[hasUpperStraight & hasLeftStraight] != 0 &&
           this->constrainRepository[hasUpperStraight & hasRightStraight] != 0 &&
           this->constrainRepository[hasDownStraight & hasLeftStraight] != 0 &&
           this->constrainRepository[hasDownStraight & hasRightStraight] != 0;
}

void BasicPieceManager::printMissingCorners(ofstream &fout) const {
    if (hasAllCorners())
        return;
    const char *message = "Cannot solve puzzle: missing corner element: ";
    if (this->constrainRepository[hasUpperStraight & hasLeftStraight] == 0) fout << message << "TL" << endl;
    if (this->constrainRepository[hasUpperStraight & hasRightStraight] == 0) fout << message << "TR" << endl;
    if (this->constrainRepository[hasDownStraight & hasLeftStraight] == 0) fout << message << "BL" << endl;
    if (this->constrainRepository[hasDownStraight & hasRightStraight] == 0) fout << message << "BR" << endl;
}

void BasicPieceManager::addPieceToRepository(Piece_t piece) {
    if (this->pieceRepository[piece]++ == 0)
        this->addPieceToConstrain(piece);
}

void BasicPieceManager::removePieceFromRepository(Piece_t piece) {
    if (--this->pieceRepository[piece] == 0)
        this->removePieceFromConstrain(piece);
}

void BasicPieceManager::addPieceToConstrain(Piece_t piece) {
    for (Piece_t maskOption : maskOptions)
        ++this->constrainRepository[piece | maskOption];
}

void BasicPieceManager::removePieceFromConstrain(Piece_t piece) {
    for (Piece_t maskOption : maskOptions)
        --this->constrainRepository[piece | maskOption];
}

void BasicPieceManager::printPiece(Piece_t piece, ofstream &out) {
    for (auto &pieceTuple : this->pieces) {
        if (get<0>(pieceTuple).representor() == piece && !get<1>(pieceTuple)) {
            get<1>(pieceTuple) = true;
            out << get<0>(pieceTuple).index;
            return;
        }
    }
    cerr << "Error: couldn't find piece: " << int(piece) << endl;
}

inline bool BasicPieceManager::pieceExistInRepository(Piece_t piece) const {
    return this->pieceRepository[piece] > 0;
}

unique_ptr<AbstractPieceManager> BasicPieceManager::clone() const {
    return unique_ptr<AbstractPieceManager>(new BasicPieceManager());
}

