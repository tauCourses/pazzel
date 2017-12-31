#include "AbstractPieceManager.h"

Piece_t AbstractPieceManager::nextPieceWithConstrain[numberOfConstrains][numberOfConstrains]; //constrain*pieces
Piece_t AbstractPieceManager::maskOptions[1 << 4];

int AbstractPieceManager::getNumOfOccurrences(int id) const {
    int occurrences = 0;
    for (auto piece : this->pieces)
        if (piece.index == id)
            occurrences++;

    return occurrences;
}

bool AbstractPieceManager::hasErrors(const PieceRepository &pieceRepository) {
    this->noPossibleShape = this->getAllPossiblePuzzleShapes().empty();
    this->pieceSumNotZero = !this->hasASumOfZero();

    return this->noPossibleShape || this->pieceSumNotZero || !this->hasAllCorners(pieceRepository);
}

void AbstractPieceManager::exportErrors(const PieceRepository &pieceRepository, ofstream &fout) const {
    this->printNoPossibleShape(fout);
    this->printMissingCorners(pieceRepository, fout);
    this->printSumNotZero(fout);
}

void AbstractPieceManager::printNoPossibleShape(ofstream &fout) const {
    if (this->noPossibleShape)
        fout << "Cannot solve puzzle: wrong number of straight edges" << endl;
}

void AbstractPieceManager::printSumNotZero(ofstream &fout) const {
    if (this->pieceSumNotZero)
        fout << "Cannot solve puzzle: sum of edges is not zero" << endl;
}

void AbstractPieceManager::initialNextPieceTable() const {
    for (int constrain = 0; constrain <= nullPiece; constrain++) {
        auto last = nullPiece;
        for (Piece_t next = 0; next < nullPiece; next++) {
            if (!PuzzlePiece::isValidPiece(next))
                continue;
            if (PuzzlePiece::isPieceRespectConstrain(next, static_cast<Piece_t>(constrain))) {
                AbstractPieceManager::nextPieceWithConstrain[constrain][last] = next;
                last = next;
            }
        }
        AbstractPieceManager::nextPieceWithConstrain[constrain][last] = nullPiece;
    }
}

void AbstractPieceManager::initialMaskOptionTable() const {
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
        AbstractPieceManager::maskOptions[mask] = maskPiece;
    }
}

AbstractPieceManager::AbstractPieceManager() {
    this->initialNextPieceTable();
    this->initialMaskOptionTable();
}

Piece_t AbstractPieceManager::getNextPiece(PieceRepository &pieceRepository, Piece_t constrain, Piece_t last) const {
    if (last != nullPiece)
        addPieceToRepository(pieceRepository, last);
    while (true) {
        Piece_t next = AbstractPieceManager::nextPieceWithConstrain[constrain][last];
        if (next == nullPiece)
            return nullPiece;
        if (this->pieceExistInRepository(pieceRepository, next)) {
            removePieceFromRepository(pieceRepository, next);
            return next;
        }
        last = next;
    }
}

bool AbstractPieceManager::hasASumOfZero() const {
    int sum = 0;
    for (auto piece : this->pieces)
        sum += piece.right + piece.left + piece.down + piece.up;
    return sum == 0;
}

bool AbstractPieceManager::preConditions() const { //default implementation, could be override
    return true;
}

int AbstractPieceManager::getNumberOfPieces() const {
    return (int) this->pieces.size();
}
