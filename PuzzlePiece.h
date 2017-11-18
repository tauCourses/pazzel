//
// Created by private on 11/11/17.
//

#ifndef PUZZLE_PUZZLE_PIECE_H
#define PUZZLE_PUZZLE_PIECE_H


#include <cstdint>
#include <stdint.h>

#define Piece_t uint8_t
#define nullPiece 0xFF

class PuzzlePiece {

public:
    PuzzlePiece(int index, int left, int up, int right, int down);

    const int index;
    const int left, up, down, right;

    Piece_t representor();
};


#endif //PUZZLE_PUZZLE_PIECE_H
