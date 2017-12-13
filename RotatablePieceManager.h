#ifndef PUZZLE_ROTATABLEPIECEMANAGER_H
#define PUZZLE_ROTATABLEPIECEMANAGER_H

#include "AbstractPieceManager.h"

class RotatablePieceManager : public AbstractPieceManager{
private:
    Piece_t lookupTable[256]; //translate Piece_t to generic Permutation
    int pieceRepository[256] = {0}; //hold the number in each Permutation
    Piece_t nextPieceWithConstrain[256][256]; //constrain*pieces
    Piece_t constrainRepository[256] = {0};
    Piece_t maskOptions[1 << 4];

    bool isPuzzleShapePossible(Shape shape) override;

    int numberOfCorners();

    void initialLookupTable();
    void initialNextPieceTable();
    void initialMaskOptionTable();
    bool isPermutation(Piece_t p1, Piece_t p2);

    void addPiece(Piece_t piece);

    void removePiece(Piece_t next);


    void removeFromConstrainRepository(Piece_t piece);

    void addToConstrainRepository(Piece_t piece);

    //error hundeling:
    bool hasASumOfZero() override;

    bool hasAllCorners() override;

    void printMissingCorners(ofstream& fout) override;

public:
    RotatablePieceManager();
    vector<AbstractPieceManager::Shape> getAllPossiblePuzzleShapes() override;

    Piece_t getNextPiece(Piece_t constrain, Piece_t last) override;
    int numOfOptionsForConstrain(Piece_t constrain) override;

    void addPiece(unique_ptr<PuzzlePiece> piece) override ;
};


#endif //PUZZLE_ROTATABLEPIECEMANAGER_H
