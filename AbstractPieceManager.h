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

    struct PieceRepository {
        int pieceRepository[numberOfConstrains] = {0};
        Piece_t constrainRepository[numberOfConstrains] = {0};
    };

    virtual vector<Shape> getAllPossiblePuzzleShapes() const = 0;

    Piece_t getNextPiece(PieceRepository &pieceRepository, Piece_t constrain, Piece_t last) const;

    virtual int numOfOptionsForConstrain(const PieceRepository &pieceRepository, Piece_t constrain) const = 0;

    virtual void addPiece(PieceRepository &pieceRepository, unique_ptr<PuzzlePiece> piece) = 0;

    int getNumOfOccurrences(int id) const;

    int getNumberOfPieces() const;

    bool hasErrors(const PieceRepository &pieceRepository);

    void exportErrors(const PieceRepository &pieceRepository, ofstream &fout) const;

    virtual void printPiece(Piece_t piece, ofstream &out) = 0;

    virtual bool preConditions() const;

    static Piece_t nextPieceWithConstrain[numberOfConstrains][numberOfConstrains]; //constrain*pieces
    static Piece_t maskOptions[1 << 4];

protected:
    vector<PuzzlePiece> pieces;
    bool noPossibleShape = false;
    bool pieceSumNotZero = false;

    AbstractPieceManager();

    void initialNextPieceTable() const;

    void initialMaskOptionTable() const;

    virtual void addPieceToRepository(PieceRepository &pieceRepository, Piece_t piece) const = 0;

    virtual void removePieceFromRepository(PieceRepository &pieceRepository, Piece_t piece) const = 0;

    virtual bool pieceExistInRepository(const PieceRepository &pieceRepository, Piece_t piece) const =0;

    virtual bool isPuzzleShapePossible(Shape shape) const = 0;

    //error handling:
    bool hasASumOfZero() const;

    virtual bool hasAllCorners(const PieceRepository &pieceRepository) const = 0;

    void printNoPossibleShape(ofstream &fout) const;

    virtual void printMissingCorners(const PieceRepository &pieceRepository, ofstream &fout) const = 0;

    void printSumNotZero(ofstream &fout) const;
};


#endif //PUZZLE_PIECEMANAGER_H
