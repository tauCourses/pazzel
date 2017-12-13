#include "BasicPieceManager.h"

void BasicPieceManager::addPiece(unique_ptr<PuzzlePiece> piece) {
    this->pieces.emplace_back(piece.get());
}

int BasicPieceManager::countConstrainOptions(Piece_t constrain) {
    return 0;
}

int BasicPieceManager::countConstrainPiece(Piece_t constrain) {
    return 0;
}

Piece_t BasicPieceManager::getNextPiece(Piece_t constrain, Piece_t last) {
    return 0;
}

vector<AbstractPieceManager::Shape> BasicPieceManager::getAllPossiblePuzzleShapes() {
    return vector<AbstractPieceManager::Shape>();
}

bool BasicPieceManager::isPuzzleShapePossible(AbstractPieceManager::Shape shape) {
    return false;
}

bool BasicPieceManager::hasASumOfZero() {
    return false;
}

bool BasicPieceManager::hasAllCorners() {
    return false;
}

void BasicPieceManager::printMissingCorners(ofstream &fout) {

}
