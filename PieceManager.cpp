//
// Created by private on 11/11/17.
//

#include "PieceManager.h"

PieceManager::PieceManager() {
    for (int mask = 0; mask < (1 << 4); ++mask) {
        maskOptions[mask] =
                static_cast<uint8_t>(((mask & (1 << 3)) != 0) * (0b11u << 6) |
                                     ((mask & (1 << 2)) != 0) * (0b11u << 4) |
                                     ((mask & (1 << 1)) != 0) * (0b11u << 2) |
                                     ((mask & (1 << 0)) != 0) * (0b11u));
    }
}

PieceManager::PieceManager(ParsedPuzzle &puzzle) : PieceManager() {
    for (int i = 0; i < puzzle.numberOfPieces; ++i) {
        addPiece(puzzle.pieces[i]->representor());
    }
}

void PieceManager::addPiece(Piece_t piece) {
    for (Piece_t maskOption : maskOptions) {
        ++count[piece & maskOption];
    }
}

void PieceManager::removePiece(Piece_t piece) {
    for (Piece_t maskOption : maskOptions) {
        --count[piece & maskOption];
    }
}

Piece_t PieceManager::getNextPiece(Piece_t constrain, Piece_t last) {
    if (last != nullPiece) {
        addPiece(last);
    }
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
    } else {
        currentLeft = currentUp = currentRight = currentDown = 0b01;
    }

    if (startLeft == 0) {
        endLeft = 0b11;
        startLeft = 0b01;
    } else {
        endLeft = currentLeft = startLeft;
    }
    if (startRight == 0) {
        endRight = 0b11;
        startRight = 0b01;
    } else {
        endRight = currentRight = startRight;
    }
    if (startUp == 0) {
        endUp = 0b11;
        startUp = 0b01;
    } else {
        endUp = currentUp = startUp;
    }
    if (startDown == 0) {
        endDown = 0b11;
        startDown = 0b01;
    } else {
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
