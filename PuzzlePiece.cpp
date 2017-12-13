#include "PuzzlePiece.h"

PuzzlePiece::PuzzlePiece(int index, int left, int up, int right, int down) :
        index(index), left(left), up(up), down(down), right(right) {}

Piece_t PuzzlePiece::representor() {
    return (Piece_t) ( (left << 6) | (up << 4) | (right << 2) | down );
}

PuzzlePiece::PuzzlePiece(const PuzzlePiece *copy) :
        index(copy->index), left(copy->left), up(copy->up), down(copy->down), right(copy->right) {}

bool PuzzlePiece::isValidPiece(Piece_t piece) {
    return (0x3 << 6 & piece) != (0x3 << 6) &&
           (0x3 << 4 & piece) != (0x3 << 4) &&
           (0x3 << 2 & piece) != (0x3 << 2) &&
           (0x3 << 0 & piece) != (0x3 << 0);
}

bool PuzzlePiece::isPieceRespectConstrain(Piece_t p, Piece_t c) {
    Piece_t p1 = static_cast<Piece_t>(p >> 6 & 0x3);
    Piece_t p2 = static_cast<Piece_t>(p >> 4 & 0x3);
    Piece_t p3 = static_cast<Piece_t>(p >> 2 & 0x3);
    Piece_t p4 = static_cast<Piece_t>(p >> 0 & 0x3);
    Piece_t c1 = static_cast<Piece_t>(c >> 6 & 0x3);
    Piece_t c2 = static_cast<Piece_t>(c >> 4 & 0x3);
    Piece_t c3 = static_cast<Piece_t>(c >> 2 & 0x3);
    Piece_t c4 = static_cast<Piece_t>(c >> 0 & 0x3);
    return (c1 == 0x3 || c1 == p1) &&
           (c2==0x3 || c2 == p2) &&
           (c3==0x3 || c3 == p3) &&
           (c4==0x3 || c4 == p4);
}
