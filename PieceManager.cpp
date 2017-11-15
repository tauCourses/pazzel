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

    if (last == nullPiece)
        last = 0;
    else
        last++;

    for (; last < nullPiece; last++) {
        if ((last & constrain) == last && valid(last) && count[last]) {
            removePiece(last);
            return last;
        }
    }

    return nullPiece;
}

int PieceManager::countConstrainPiece(Piece_t constrain) {
    return count[constrain];
}

int PieceManager::countConstrainOptions(Piece_t constrain) {
    return countOptions[constrain];
}