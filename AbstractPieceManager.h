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

    virtual vector<Shape> getAllPossiblePuzzleShapes() = 0;

    virtual Piece_t getNextPiece(Piece_t constrain, Piece_t last) = 0;
    virtual int countConstrainPiece(Piece_t constrain) = 0;
    virtual int countConstrainOptions(Piece_t constrain) = 0;

    virtual void addPiece(unique_ptr<PuzzlePiece> piece) = 0;
    int checkPieceIdExistOnce(int id) const;

    bool hasErrors();

    void exportErrors(ofstream& fout);

private:
    vector<PuzzlePiece> pieces;

    virtual bool isPuzzleShapePossible(Shape shape) = 0;

    //error hundeling:
    virtual bool hasASumOfZero() = 0;

    virtual bool hasAllCorners() = 0;

    bool noPossibleShape = false;
    bool pieceSumNotZero = false;

    void printNoPissibleShape(ofstream& fout);
    virtual void printMissingCorners(ofstream& fout) = 0;
    void printSumNotZero(ofstream& fout);


};


#endif //PUZZLE_PIECEMANAGER_H
