#include "AbstractPieceManager.h"

Piece_t AbstractPieceManager::nextPieceWithConstrain[numberOfConstrains][numberOfConstrains]; //constrain*pieces
Piece_t AbstractPieceManager::maskOptions[1 << 4];

int AbstractPieceManager::getNumOfOccurrences(int id) const {
    int occurrences = 0;
    for (auto piece : this->pieces)
        if (get<0>(piece).index == id)
            occurrences++;

    return occurrences;
}

bool AbstractPieceManager::hasErrors() {
    this->noPossibleShape = this->getAllPossiblePuzzleShapes().empty();
    this->pieceSumNotZero = !this->hasASumOfZero();

    return this->noPossibleShape || this->pieceSumNotZero || !this->hasAllCorners();
}

void AbstractPieceManager::exportErrors(ofstream &fout) const {
    this->printNoPossibleShape(fout);
    this->printMissingCorners(fout);
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

Piece_t AbstractPieceManager::getNextPiece(Piece_t constrain, Piece_t last) {
    Piece_t next;
    if (last != nullPiece)
        this->addPieceToRepository(last);
    while (true) {
        next = AbstractPieceManager::nextPieceWithConstrain[constrain][last];
        if (next == nullPiece)
            return nullPiece;
        if (this->pieceExistInRepository(next)) {
            this->removePieceFromRepository(next);
            return next;
        }
        last = next;
    }
}

bool AbstractPieceManager::hasASumOfZero() const {
    int sum = 0;
    for (auto pieceTuple : this->pieces) {
        auto piece = get<0>(pieceTuple);
        sum += piece.right + piece.left + piece.down + piece.up;
    }return sum == 0;
}

void AbstractPieceManager::preConditions() const { //default implementation, could be override
    if(this->hasErrors())
        this->throwException();
}

unsigned int AbstractPieceManager::getNumberOfPieces() const {
    return this->pieces.size();
}

void AbstractPieceManager::retrieveData(const unique_ptr<AbstractPieceManager> &basePieceManager) {
    std::copy(basePieceManager->constrainRepository,
              basePieceManager->constrainRepository + (int) (numberOfConstrains),
              this->constrainRepository);

    std::copy(basePieceManager->pieceRepository,
              basePieceManager->pieceRepository + (int) (numberOfConstrains),
              this->pieceRepository);
}

Piece_t AbstractPieceManager::getConstrainOpposite(Piece_t currentConstrain) {
    return static_cast<uint8_t>((((currentConstrain & 0x1) == (uint8_t) (currentConstrain >> 1)) << 1) |
                                (currentConstrain & 0x1));
}