//
// Created by private on 11/11/17.
//

#ifndef PAZZEL_PUZZLEPIECE_H
#define PAZZEL_PUZZLEPIECE_H


#include <cstdint>

#define Piece_t uint8_t

class PuzzlePiece {
    int index;
    int up, down, left, right;

public:
    Piece_t representor();
};



#endif //PAZZEL_PUZZLEPIECE_H
