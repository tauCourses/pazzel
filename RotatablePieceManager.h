#ifndef PUZZLE_ROTATABLEPIECEMANAGER_H
#define PUZZLE_ROTATABLEPIECEMANAGER_H

#include "AbstractPieceManager.h"
#include "PuzzlePieceConstrain.h"

class RotatablePieceManager : public AbstractPieceManager {
private:
    Piece_t lookupTable[numberOfConstrains]; //translate Piece_t to generic Permutation

    bool isPuzzleShapePossible(Shape shape) override;

    int numberOfCorners();

    void initialLookupTable();

    bool isPermutation(Piece_t p1, Piece_t p2);

    int getPermutationDegree(Piece_t current, Piece_t origin);

    void removeFromConstrainRepository(Piece_t piece);

    void addToConstrainRepository(Piece_t piece);

    void changeConstrains(Piece_t piece, int delta);

    inline Piece_t rotatePieceCounterClockWise(Piece_t piece);

protected:

    void addPieceToRepository(Piece_t piece) override;

    void removePieceFromRepository(Piece_t piece) override;

    bool pieceExistInRepository(Piece_t piece) override;

    //error hundeling:
    bool hasASumOfZero() override;

    bool hasAllCorners() override;
    bool hasTwoSideForARaw();

    void printMissingCorners(ofstream &fout) override;

public:
    RotatablePieceManager();

    vector<AbstractPieceManager::Shape> getAllPossiblePuzzleShapes() override;

    int numOfOptionsForConstrain(Piece_t constrain) override;

    void addPiece(unique_ptr<PuzzlePiece> piece) override;

    void printPiece(Piece_t piece, ofstream &out) override;


};


#endif //PUZZLE_ROTATABLEPIECEMANAGER_H
