#include "AbstractPieceManager.h"

int AbstractPieceManager::checkPieceIdExistOnce(int id) const {
    int occurrences = 0;
    for (auto piece : this->pieces)
        if (piece.index == id)
            occurrences++;

    return occurrences;
}

bool AbstractPieceManager::hasErrors() {
    this->noPossibleShape = this->getAllPossiblePuzzleShapes().empty();
    this->pieceSumNotZero = !this->hasASumOfZero();

    return this->noPossibleShape ||
           this->pieceSumNotZero ||
           !this->hasAllCorners();
}

void AbstractPieceManager::exportErrors(ofstream &fout) {
    this->printNoPossibleShape(fout);
    this->printMissingCorners(fout);
    this->printSumNotZero(fout);
}

void AbstractPieceManager::printNoPossibleShape(ofstream &fout) {
    if (this->noPossibleShape)
        fout << "Cannot solve puzzle: wrong number of straight edges" << endl;
}

void AbstractPieceManager::printSumNotZero(ofstream &fout) {
    if (this->pieceSumNotZero)
        fout << "Cannot solve puzzle: sum of edges is not zero" << endl;
}

void AbstractPieceManager::initialNextPieceTable() {
    for (int constrain = 0; constrain <= nullPiece; constrain++) {
        auto last = nullPiece;
        for (Piece_t next = 0; next < nullPiece; next++) {
            if (!PuzzlePiece::isValidPiece(next))
                continue;
            if (PuzzlePiece::isPieceRespectConstrain(next, static_cast<Piece_t>(constrain))) {
                this->nextPieceWithConstrain[constrain][last] = next;
                last = next;
            }
        }
        this->nextPieceWithConstrain[constrain][last] = nullPiece;
    }
}

void AbstractPieceManager::initialMaskOptionTable() {
    Piece_t maskPiece;
    for (int mask = 0; mask < (1 << 4); ++mask) {
        maskPiece = 0;
        if ((mask & (1 << 3)) == 0)
            maskPiece |= (0x3 << 6);
        if ((mask & (1 << 2)) == 0)
            maskPiece |= (0x3 << 4);
        if ((mask & (1 << 1)) == 0)
            maskPiece |= (0x3 << 2);
        if ((mask & (1 << 0)) == 0)
            maskPiece |= (0x3 << 0);
        this->maskOptions[mask] = maskPiece;
    }
}

AbstractPieceManager::AbstractPieceManager() {
    this->initialNextPieceTable();
    this->initialMaskOptionTable();
}

Piece_t AbstractPieceManager::getNextPiece(Piece_t constrain, Piece_t last) {
    if (last != nullPiece)
        addPieceToRepository(last);
    while (true) {
        Piece_t next = this->nextPieceWithConstrain[constrain][last];
        if (next == nullPiece)
            return nullPiece;
        if (this->pieceExistInRepository(next)) {
            removePieceFromRepository(next);
            return next;
        }
        last = next;
    }
}

bool AbstractPieceManager::hasASumOfZero() {
    int sum = 0;
    for (auto piece : this->pieces)
        sum += piece.right + piece.left + piece.down + piece.up;
    return sum == 0;
}

bool AbstractPieceManager::preConditions() { //default implementation, could be override
    return true;
}
