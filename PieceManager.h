//
// Created by private on 11/11/17.
//

#ifndef PAZZEL_PIECEMANAGER_H
#define PAZZEL_PIECEMANAGER_H

#include "ParsedPuzzle.h"
#include "PuzzlePiece.h"

#define NumberOfPossibleConstrains 1<<8

class PieceManager {
    int count[NumberOfPossibleConstrains] = {0};
    Piece_t maskOptions[1 << 4];

    void addPiece(Piece_t piece);

    void removePiece(Piece_t piece);

public:
    PieceManager();

    explicit PieceManager(ParsedPuzzle &puzzle);

    Piece_t getNextPiece(Piece_t constrain, Piece_t last);

    int countConstrainPiece(Piece_t constrain);
};


#endif //PAZZEL_PIECEMANAGER_H
