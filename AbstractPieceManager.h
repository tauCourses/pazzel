#ifndef PUZZLE_PIECEMANAGER_H
#define PUZZLE_PIECEMANAGER_H

#include "ParsedPuzzle.h"
#include "PuzzlePiece.h"

#define NumberOfPossibleConstrains 1<<8

#include <stdint.h>

#define Piece_t uint8_t
#define nullPiece (Piece_t) 0xFF //0b11111111

class AbstractPieceManager {


    class PuzzlePiece {

    public:
        PuzzlePiece(int index, int left, int up, int right, int down);

        const int index;
        const int left, up, down, right;

        Piece_t representor();
    };

private:
    ParsingErrors errors;
    PuzzlePiece **pieces = nullptr;

    virtual void hasWrongNumberOfStraightLines(int numberOfPieces);

    virtual void hasASumOfZero();

    virtual void hasAllCorners();

    void parsePiecesFromFile(char* fileName);

public:
    AbstractPieceManager();

    virtual Piece_t getNextPiece(Piece_t constrain, Piece_t last) = 0;
    virtual bool isPuzzleShapePossible(int numberOfRows, int numberOfCols);
    virtual void addPiece(PuzzlePiece);
    int countConstrainPiece(Piece_t constrain);
    int countConstrainOptions(Piece_t constrain);
};


#endif //PUZZLE_PIECEMANAGER_H
