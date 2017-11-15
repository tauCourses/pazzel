//
// Created by private on 11/11/17.
//

#include "PieceManager.h"

PieceManager::PieceManager() {
    Piece_t maskPiece;
    for (int mask = 0; mask < (1 << 4); ++mask) {
        maskPiece = 0;
        if ((mask & (1 << 3)) == 0) maskPiece |= (0b11u << 6);
        if ((mask & (1 << 2)) == 0) maskPiece |= (0b11u << 4);
        if ((mask & (1 << 1)) == 0) maskPiece |= (0b11u << 2);
        if ((mask & (1 << 0)) == 0) maskPiece |= (0b11u << 0);
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
        return (0b11 << 6 & piece) != (0b11 << 6) &&
               (0b11 << 4 & piece) != (0b11 << 4) &&
               (0b11 << 2 & piece) != (0b11 << 2) &&
               (0b11 << 0 & piece) != (0b11 << 0);
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

	startLeft = (constrain >> 6) & 0b11;
	startUp = (constrain >> 4) & 0b11;
	startRight = (constrain >> 2) & 0b11;
	startDown = (constrain >> 0) & 0b11;

	if (last != nullPiece) {
		currentLeft = (last >> 6) & 0b11;
		currentUp = (last >> 4) & 0b11;
		currentRight = (last >> 2) & 0b11;
		currentDown = (last >> 0) & 0b11;
	}
	else {
		currentLeft = currentUp = currentRight = currentDown = 0b00;
	}

	if (startLeft == 0b11) {
		endLeft = 0b10;
		startLeft = 0b00;
	}
	else {
		endLeft = currentLeft = startLeft;
	}
	if (startRight == 0b11) {
		endRight = 0b10;
		startRight = 0b00;
	}
	else {
		endRight = currentRight = startRight;
	}
	if (startUp == 0b11) {
		endUp = 0b10;
		startUp = 0b00;
	}
	else {
		endUp = currentUp = startUp;
	}
	if (startDown == 0b11) {
		endDown = 0b10;
		startDown = 0b00;
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