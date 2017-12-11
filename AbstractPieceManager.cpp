#include "AbstractPieceManager.h"

AbstractPieceManager::AbstractPieceManager() {
    Piece_t maskPiece;
    for (int mask = 0; mask < (1 << 4); ++mask) {
        maskPiece = 0;
        if ((mask & (1 << 3)) == 0) maskPiece |= (0x3 << 6);
        if ((mask & (1 << 2)) == 0) maskPiece |= (0x3 << 4);
        if ((mask & (1 << 1)) == 0) maskPiece |= (0x3 << 2);
        if ((mask & (1 << 0)) == 0) maskPiece |= (0x3 << 0);
        maskOptions[mask] = maskPiece;
    }
}

AbstractPieceManager::AbstractPieceManager(ParsedPuzzle &puzzle) : AbstractPieceManager() {
    for (int i = 0; i < puzzle.numberOfPieces; ++i)
        this->addPiece(puzzle.pieces[i]->representor());
}

void AbstractPieceManager::addPiece(Piece_t piece) {
    for (Piece_t maskOption : maskOptions)
        ++this->piecesCounter[piece | maskOption];

    if (this->piecesCounter[piece] == 1)
        this->addOption(piece);

}

void AbstractPieceManager::removeOption(Piece_t piece) {
    for (Piece_t maskOption : maskOptions)
        --countOptions[piece | maskOption];

}

void AbstractPieceManager::addOption(Piece_t piece) {
    for (Piece_t maskOption : maskOptions)
        ++countOptions[piece | maskOption];

}

void AbstractPieceManager::removePiece(Piece_t piece) {
    for (Piece_t maskOption : maskOptions)
        --this->piecesCounter[piece | maskOption];

    if (this->piecesCounter[piece] == 0)
        this->removeOption(piece);
}


Piece_t AbstractPieceManager::getNextPiece(Piece_t constrain, Piece_t last) {
	if (last != nullPiece) {
		this->addPiece(last);
	}
	if (this->piecesCounter[constrain] == 0)
		return nullPiece;
	int startLeft, startUp, startRight, startDown,
		currentLeft, currentUp, currentRight, currentDown,
		endLeft, endUp, endRight, endDown;

	startLeft = (constrain >> 6) & 0x3;
	startUp = (constrain >> 4) & 0x3;
	startRight = (constrain >> 2) & 0x3;
	startDown = (constrain >> 0) & 0x3;

	if (last != nullPiece) {
		currentLeft = (last >> 6) & 0x3;
		currentUp = (last >> 4) & 0x3;
		currentRight = (last >> 2) & 0x3;
		currentDown = (last >> 0) & 0x3;
	}
	else {
		currentLeft = currentUp = currentRight = currentDown = 0x0;
	}

	if (startLeft == 0x3) {
		endLeft = 0x2;
		startLeft = 0x0;
	}
	else {
		endLeft = currentLeft = startLeft;
	}
	if (startRight == 0x3) {
		endRight = 0x2;
		startRight = 0x0;
	}
	else {
		endRight = currentRight = startRight;
	}
	if (startUp == 0x3) {
		endUp = 0x2;
		startUp = 0x0;
	}
	else {
		endUp = currentUp = startUp;
	}
	if (startDown == 0x3) {
		endDown = 0x2;
		startDown = 0x0;
	}
	else {
		endDown = currentDown = startDown;
	}

	Piece_t currentPiece;
	for (; currentLeft <= endLeft; ++currentLeft) {
		for (; currentUp <= endUp; ++currentUp) {
			for (; currentRight <= endRight; ++currentRight) {
				for (; currentDown <= endDown; ++currentDown) {
					currentPiece = static_cast<uint8_t>(
						(currentLeft << 6) | (currentUp << 4) | (currentRight << 2) |
						(currentDown << 0));
					if (currentPiece != last && this->piecesCounter[currentPiece] != 0) {
						removePiece(currentPiece);
						return currentPiece;
					}
				}
				currentDown = startDown;
			}
			currentRight = startRight;
		}
		currentUp = startUp;
	}
	return nullPiece;
}

int AbstractPieceManager::countConstrainPiece(Piece_t constrain) {
    return this->piecesCounter[constrain];
}

int AbstractPieceManager::countConstrainOptions(Piece_t constrain) {
    return countOptions[constrain];
}