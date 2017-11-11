//
// Created by private on 11/11/17.
//

#ifndef PAZZEL_PIECEMANAGER_H
#define PAZZEL_PIECEMANAGER_H

#include "parsedData.h"
#include "PuzzlePiece.h"

class PieceManager {
    public:
        PieceManager(parsedData data);
        Piece_t getPiece(Piece_t constrain, Piece_t last);
        int countConstrainPiece(Piece_t constrain)
};


#endif //PAZZEL_PIECEMANAGER_H
