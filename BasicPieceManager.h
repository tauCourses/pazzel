#ifndef PUZZLE_BASICPIECEMANAGER_H
#define PUZZLE_BASICPIECEMANAGER_H

#include "AbstractPieceManager.h"

class BasicPieceManager : public AbstractPieceManager {
public:
    vector<Shape> getAllPossiblePuzzleShapes();

    Piece_t getNextPiece(Piece_t constrain, Piece_t last);
    int countConstrainPiece(Piece_t constrain);
    int countConstrainOptions(Piece_t constrain);

    void addPiece(unique_ptr<PuzzlePiece> piece);

private:
    bool isPuzzleShapePossible(Shape shape);

    //error hundeling:
    bool hasASumOfZero();

    bool hasAllCorners();

    virtual void printMissingCorners(ofstream& fout);
};


#endif //PUZZLE_BASICPIECEMANAGER_H
