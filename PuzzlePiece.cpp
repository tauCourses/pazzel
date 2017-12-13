#include "PuzzlePiece.h"

PuzzlePiece::PuzzlePiece(int index, int left, int up, int right, int down) :
        index(index), left(left), up(up), down(down), right(right) {}

Piece_t PuzzlePiece::representor() {
    return (Piece_t) ( (left << 6) | (up << 4) | (right << 2) | down );
}

PuzzlePiece::PuzzlePiece(const PuzzlePiece *copy) :
        index(copy->index), left(copy->left), up(copy->up), down(copy->down), right(copy->right) {}