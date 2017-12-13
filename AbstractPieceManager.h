#ifndef PUZZLE_PIECEMANAGER_H
#define PUZZLE_PIECEMANAGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include "PuzzlePiece.h"

using namespace std;


class AbstractPieceManager {

public:
    struct Shape {
        int width, height;
    };

    virtual vector<AbstractPieceManager::Shape> getAllPossiblePuzzleShapes() = 0;

    virtual Piece_t getNextPiece(Piece_t constrain, Piece_t last) = 0;

    virtual int numOfOptionsForConstrain(Piece_t constrain) = 0;

    virtual void addPiece(unique_ptr<PuzzlePiece> piece) = 0;

    int checkPieceIdExistOnce(int id) const;

    bool hasErrors();

    void exportErrors(ofstream &fout);

    virtual void addPieceToCount(Piece_t piece) = 0;

    virtual void removePieceFromCount(Piece_t piece) = 0;

    virtual unique_ptr<PuzzlePiece> getPieceOfType(Piece_t piece) = 0;

protected:
    vector<PuzzlePiece> pieces;

    virtual bool isPuzzleShapePossible(Shape shape) = 0;

    //error handling:
    virtual bool hasASumOfZero() = 0;

    virtual bool hasAllCorners() = 0;

    bool noPossibleShape = false;
    bool pieceSumNotZero = false;

    void printNoPossibleShape(ofstream &fout);

    virtual void printMissingCorners(ofstream &fout) = 0;

    void printSumNotZero(ofstream &fout);
};


#endif //PUZZLE_PIECEMANAGER_H
