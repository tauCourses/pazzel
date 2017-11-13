//
// Created by private on 11/11/17.
//

#include "PuzzlePiece.h"

PuzzlePiece::PuzzlePiece(int index, int left, int up, int right, int down) :
        index(index), left(left), up(up), down(down), right(right) {}

Piece_t PuzzlePiece::representor() {
    return (Piece_t) ( (left << 6) | (up << 4) | (right << 2) | down );
}
