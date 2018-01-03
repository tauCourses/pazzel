#ifndef PUZZLE_PIECEMANAGER_H
#define PUZZLE_PIECEMANAGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <sstream>

#include "PuzzlePiece.h"
#include "PuzzlePieceConstrain.h"
#include "PuzzleException.h"

using namespace std;

class AbstractPieceManager {

public:
    struct Shape {
        int width, height;
    };

    virtual vector<Shape> getAllPossiblePuzzleShapes() const = 0;

    Piece_t getNextPiece(Piece_t constrain, Piece_t last);

    virtual int numOfOptionsForConstrain(Piece_t constrain) const = 0;

    virtual void addPiece(unique_ptr<PuzzlePiece> piece) = 0;

    int getNumOfOccurrences(int id) const;

    unsigned long getNumberOfPieces() const;

    virtual void checkPreConditions();

    virtual void printPiece(Piece_t piece, ofstream &out) = 0;

    void retrieveData(const unique_ptr<AbstractPieceManager> &prototypePieceManager);

    virtual unique_ptr<AbstractPieceManager> clone() const = 0;

    static Piece_t getConstrainOpposite(Piece_t currentConstrain);

    static Piece_t nextPieceWithConstrain[numberOfConstrains][numberOfConstrains]; //constrain*pieces
    static Piece_t maskOptions[1 << 4];

protected:

    vector<std::tuple<PuzzlePiece,bool>> pieces; //the boolean represent if it used for solution. just for printing!
    bool noPossibleShape = false;
    bool pieceSumNotZero = false;
    int pieceRepository[numberOfConstrains] = {0};
    Piece_t constrainRepository[numberOfConstrains] = {0};

    AbstractPieceManager();

    void initialNextPieceTable() const;

    void initialMaskOptionTable() const;

    virtual void addPieceToRepository(Piece_t piece)  = 0;

    virtual void removePieceFromRepository(Piece_t piece)  = 0;

    virtual bool pieceExistInRepository(Piece_t piece) const =0;

    virtual bool isPuzzleShapePossible(Shape shape) const = 0;

    //error handling:
    bool hasErrors();

    void throwException() const;

    bool hasASumOfZero() const;

    virtual bool hasAllCorners() const = 0;

    void printNoPossibleShape(stringstream &fout) const;

    virtual void printMissingCorners(stringstream &fout) const = 0;

    void printSumNotZero(stringstream &fout) const;
};


#endif //PUZZLE_PIECEMANAGER_H
