#ifndef PUZZLE_BASICPIECEMANAGER_H
#define PUZZLE_BASICPIECEMANAGER_H

#include "AbstractPieceManager.h"
#include "PuzzlePieceConstrain.h"
#include <memory>

class BasicPieceManager : public AbstractPieceManager {
public:
    BasicPieceManager();

    vector<Shape> getAllPossiblePuzzleShapes() override;

    Piece_t getNextPiece(Piece_t constrain, Piece_t last) override;

    int numOfOptionsForConstrain(Piece_t constrain) override;

    void addPiece(unique_ptr<PuzzlePiece> piece) override;

    void addPieceToCount(Piece_t piece) override;

    inline void removePieceFromCount(Piece_t piece) override;

    unique_ptr<PuzzlePiece> getPieceOfType(Piece_t piece) override;

private:
    void addPieceToOptions(Piece_t piece);

    void removePieceFromOptions(Piece_t piece);

    bool isPuzzleShapePossible(Shape shape) override;

    void createMaskVector();

    void createNextPieceVector();

    vector<Piece_t> maskVector;
    vector<vector<Piece_t>> nextPieceVector;

    vector<int> constrainCount;

    vector<int> constrainOption;

    //error hundeling:
    bool hasASumOfZero() override;

    bool hasAllCorners() override;

    void printMissingCorners(ofstream &fout) override;
};


#endif //PUZZLE_BASICPIECEMANAGER_H
