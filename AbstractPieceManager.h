#ifndef PUZZLE_PIECEMANAGER_H
#define PUZZLE_PIECEMANAGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>

#include "PuzzlePiece.h"
#include "PuzzlePieceConstrain.h"

using namespace std;

class AbstractPieceManager {

public:
    struct Shape {
        int width, height;
    };


    virtual vector<AbstractPieceManager::Shape> getAllPossiblePuzzleShapes() = 0;
    virtual unique_ptr<AbstractPieceManager> clone() = 0;
    virtual void retrieveData(const unique_ptr<AbstractPieceManager>& basePieceManager) = 0;

    Piece_t getNextPiece(Piece_t constrain, Piece_t last);

    virtual int numOfOptionsForConstrain(Piece_t constrain) = 0;

    virtual void addPiece(unique_ptr<PuzzlePiece> piece) = 0;

    int getNumOfOccurrences(int id) const;

    int getNumberOfPieces() const;

    bool hasErrors();

    void exportErrors(ofstream &fout);

    virtual void printPiece(Piece_t piece, ofstream &out) = 0;

    virtual bool preConditions();


    static Piece_t nextPieceWithConstrain[numberOfConstrains][numberOfConstrains]; //constrain*pieces
    static Piece_t maskOptions[1 << 4];
    int pieceRepository[numberOfConstrains] = {0};
    Piece_t constrainRepository[numberOfConstrains] = {0};

protected:
    AbstractPieceManager();

    void initialNextPieceTable();

    void initialMaskOptionTable();

    virtual void addPieceToRepository(Piece_t piece) = 0;

    virtual void removePieceFromRepository(Piece_t piece) = 0;

    virtual bool pieceExistInRepository(Piece_t piece) =0;

    vector<PuzzlePiece> pieces;

    virtual bool isPuzzleShapePossible(Shape shape) = 0;

    //error handling:
    bool hasASumOfZero();

    virtual bool hasAllCorners() = 0;

    bool noPossibleShape = false;
    bool pieceSumNotZero = false;

    void printNoPossibleShape(ofstream &fout);

    virtual void printMissingCorners(ofstream &fout) = 0;

    void printSumNotZero(ofstream &fout);
};


#endif //PUZZLE_PIECEMANAGER_H
