#ifndef PUZZLE_PIECEMANAGER_H
#define PUZZLE_PIECEMANAGER_H

#include "ParsedPuzzle.h"
#include "PuzzlePiece.h"

#define NumberOfPossibleConstrains 1<<8

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <bits/unique_ptr.h>

using namespace std;

#include "PuzzlePiece.h"

class AbstractPieceManager {

public:
    struct Shape {
        int width, height;
    };

    virtual vector<Shape> getAllPossiblePuzzleShapes();

    AbstractPieceManager();

    virtual Piece_t getNextPiece(Piece_t constrain, Piece_t last) = 0;
    int countConstrainPiece(Piece_t constrain);
    int countConstrainOptions(Piece_t constrain);

    virtual void addPiece(unique_ptr<PuzzlePiece>&& piece);
    virtual const vector<PuzzlePiece>& getAllPuzzlePieces();
    void setSize(int numberOfPieses);

    bool hasErrors();

    void exportErrors(ofstream& fout);

private:
    ParsingErrors errors;
    PuzzlePiece **pieces = nullptr;

    virtual void hasWrongNumberOfStraightLines(int numberOfPieces);

    virtual void hasASumOfZero();

    virtual void hasAllCorners();

    virtual bool isPuzzleShapePossible(Shape shape);
};


#endif //PUZZLE_PIECEMANAGER_H
