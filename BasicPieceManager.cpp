#include "BasicPieceManager.h"

void BasicPieceManager::addPiece(PieceRepository &pieceRepository, unique_ptr <PuzzlePiece> piece) {
    this->pieces.emplace_back(piece.get());
    this->addPieceToRepository(pieceRepository, piece->representor());
}

inline int
BasicPieceManager::numOfOptionsForConstrain(const PieceRepository &pieceRepository, Piece_t constrain) const {
    return pieceRepository.constrainRepository[constrain];
}

vector <AbstractPieceManager::Shape> BasicPieceManager::getAllPossiblePuzzleShapes() const {
    vector <AbstractPieceManager::Shape> possibleShapes;
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
    for (auto &it : this->pieces) {
        if (it.left == 0) leftStraightCount++;
        if (it.right == 0) rightStraightCount++;
        if (it.up == 0) upStraightCount++;
        if (it.down == 0) downStraightCount++;
    }
    return leftStraightCount >= shape.height &&
           rightStraightCount >= shape.height &&
           upStraightCount >= shape.width &&
           downStraightCount >= shape.width;
}

bool BasicPieceManager::preConditions() const {
    int leftMaleRightFemaleRatio = 0, leftFemaleRightMaleRatio = 0, upMaleDownFemaleRatio = 0, upFemaleDownMaleRatio = 0;
    for (auto &it : this->pieces) {
        if (it.left == 1) leftMaleRightFemaleRatio++;
        if (it.right == -1) leftMaleRightFemaleRatio--;
        if (it.left == -1) leftFemaleRightMaleRatio++;
        if (it.right == 1) leftFemaleRightMaleRatio--;
        if (it.up == 1) upMaleDownFemaleRatio++;
        if (it.down == -1) upMaleDownFemaleRatio--;
        if (it.up == -1) upFemaleDownMaleRatio++;
        if (it.down == 1) upFemaleDownMaleRatio--;
    }
    return leftMaleRightFemaleRatio == 0 &&
           leftFemaleRightMaleRatio == 0 &&
           upMaleDownFemaleRatio == 0 &&
           upFemaleDownMaleRatio == 0;
}

bool BasicPieceManager::hasAllCorners(const PieceRepository &pieceRepository) const {
    return pieceRepository.constrainRepository[hasUpperStraight & hasLeftStraight] != 0 &&
           pieceRepository.constrainRepository[hasUpperStraight & hasRightStraight] != 0 &&
           pieceRepository.constrainRepository[hasDownStraight & hasLeftStraight] != 0 &&
           pieceRepository.constrainRepository[hasDownStraight & hasRightStraight] != 0;
}

void BasicPieceManager::printMissingCorners(const PieceRepository &pieceRepository, ofstream &fout) const {
    if (hasAllCorners(pieceRepository))
        return;
    const char *message = "Cannot solve puzzle: missing corner element: ";
    if (pieceRepository.constrainRepository[hasUpperStraight & hasLeftStraight] == 0) fout << message << "TL" << endl;
    if (pieceRepository.constrainRepository[hasUpperStraight & hasRightStraight] == 0) fout << message << "TR" << endl;
    if (pieceRepository.constrainRepository[hasDownStraight & hasLeftStraight] == 0) fout << message << "BL" << endl;
    if (pieceRepository.constrainRepository[hasDownStraight & hasRightStraight] == 0) fout << message << "BR" << endl;
}

void BasicPieceManager::addPieceToRepository(PieceRepository &pieceRepository, Piece_t piece) const {
    if (pieceRepository.pieceRepository[piece]++ == 0)
        this->addPieceToConstrain(pieceRepository, piece);
}

void BasicPieceManager::removePieceFromRepository(PieceRepository &pieceRepository, Piece_t piece) const {
    if (--pieceRepository.pieceRepository[piece] == 0)
        this->removePieceFromConstrain(pieceRepository, piece);
}

void BasicPieceManager::addPieceToConstrain(PieceRepository &pieceRepository, Piece_t piece) const {
    for (Piece_t maskOption : maskOptions)
        ++pieceRepository.constrainRepository[piece | maskOption];
}

void BasicPieceManager::removePieceFromConstrain(PieceRepository &pieceRepository, Piece_t piece) const {
    for (Piece_t maskOption : maskOptions)
        --pieceRepository.constrainRepository[piece | maskOption];
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

inline bool BasicPieceManager::pieceExistInRepository(const PieceRepository &pieceRepository, Piece_t piece) const {
    return pieceRepository.pieceRepository[piece] > 0;
}

