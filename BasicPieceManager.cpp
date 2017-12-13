#include "BasicPieceManager.h"

void BasicPieceManager::addPiece(unique_ptr<PuzzlePiece> piece) {
    this->pieces.emplace_back(piece.get());
}

int BasicPieceManager::countConstrainOptions(Piece_t constrain) {
    return constrain;
}

int BasicPieceManager::countConstrainPiece(Piece_t constrain) {
    return constrain;
}

Piece_t BasicPieceManager::getNextPiece(Piece_t constrain, Piece_t last) {
    return constrain + last;
}

vector<AbstractPieceManager::Shape> BasicPieceManager::getAllPossiblePuzzleShapes() {
    return vector<AbstractPieceManager::Shape>();
}

bool BasicPieceManager::isPuzzleShapePossible(AbstractPieceManager::Shape shape) {
    return shape.height == 7;
}

bool BasicPieceManager::hasASumOfZero() {
    return true;
}

bool BasicPieceManager::hasAllCorners() {
    return true;
}

void BasicPieceManager::printMissingCorners(ofstream &fout) {
    fout << "no way!";
}
