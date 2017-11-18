//
// Created by private on 11/11/17.
//

#include "PieceManager.h"

PieceManager::PieceManager() {
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

PieceManager::PieceManager(ParsedPuzzle &puzzle) : PieceManager() {
    for (int i = 0; i < puzzle.numberOfPieces; ++i) {
        addPiece(puzzle.pieces[i]->representor());
    }
}

void PieceManager::addPiece(Piece_t piece) {
    for (Piece_t maskOption : maskOptions) {
        ++count[piece | maskOption];
    }
    if (count[piece] == 1) {
        addOption(piece);
    }
}

void PieceManager::removeOption(Piece_t piece) {
    for (Piece_t maskOption : maskOptions) {
        --countOptions[piece | maskOption];
    }
}

void PieceManager::addOption(Piece_t piece) {
    for (Piece_t maskOption : maskOptions) {
        ++countOptions[piece | maskOption];
    }
}

void PieceManager::removePiece(Piece_t piece) {
    for (Piece_t maskOption : maskOptions) {
        --count[piece | maskOption];
    }
    if (count[piece] == 0) {
        removeOption(piece);
    }
}

namespace {
    inline bool valid(Piece_t piece) {
        return (0x3 << 6 & piece) != (0x3 << 6) &&
               (0x3 << 4 & piece) != (0x3 << 4) &&
               (0x3 << 2 & piece) != (0x3 << 2) &&
               (0x3 << 0 & piece) != (0x3 << 0);
    }
}

Piece_t PieceManager::getNextPiece(Piece_t constrain, Piece_t last) {
	if (last != nullPiece) {
		addPiece(last);
	}
	if (count[constrain] == 0) return nullPiece;
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
					if (currentPiece != last && count[currentPiece] != 0) {
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

int PieceManager::countConstrainPiece(Piece_t constrain) {
    return count[constrain];
}

int PieceManager::countConstrainOptions(Piece_t constrain) {
    return countOptions[constrain];
}