#ifndef PUZZLE_BASICPIECEMANAGER_H
#define PUZZLE_BASICPIECEMANAGER_H

#include "AbstractPieceManager.h"

class BasicPieceManager : public AbstractPieceManager {
public:
    vector<Shape> getAllPossiblePuzzleShapes() override;

    Piece_t getNextPiece(Piece_t constrain, Piece_t last) override;
    virtual int numOfOptionsForConstrain(Piece_t constrain) override;

    void addPiece(unique_ptr<PuzzlePiece> piece) override ;

private:
    bool isPuzzleShapePossible(Shape shape) override;

    //error hundeling:
    bool hasASumOfZero() override;

    bool hasAllCorners() override;

    void printMissingCorners(ofstream& fout) override;
};


#endif //PUZZLE_BASICPIECEMANAGER_H
