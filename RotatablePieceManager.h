#ifndef PUZZLE_ROTATABLEPIECEMANAGER_H
#define PUZZLE_ROTATABLEPIECEMANAGER_H

#include <algorithm>
#include "AbstractPieceManager.h"
#include "PuzzlePieceConstrain.h"

class RotatablePieceManager : public AbstractPieceManager {
public:
    RotatablePieceManager();

    vector<AbstractPieceManager::Shape> getAllPossiblePuzzleShapes() const override;

    int numOfOptionsForConstrain(Piece_t constrain) const override;

    void addPiece(unique_ptr<PuzzlePiece> piece) override;

    void printPiece(Piece_t piece, ofstream &out) override;

    unique_ptr<AbstractPieceManager> clone() const override;

protected:
    void addPieceToRepository(Piece_t piece) override;

    void removePieceFromRepository(Piece_t piece) override;

    bool pieceExistInRepository(Piece_t piece) const override;

    //error hundeling:
    bool hasAllCorners() const override;

    bool hasTwoSideForARaw() const;

    void printMissingCorners(ofstream &fout) const override;

private:
    static Piece_t lookupTable[numberOfConstrains]; //translate Piece_t to generic Permutation

    bool isPuzzleShapePossible(AbstractPieceManager::Shape shape) const override;

    int numberOfCorners() const;

    void initialLookupTable() const;

    bool isPermutation(Piece_t p1, Piece_t p2) const;

    int getPermutationDegree(Piece_t current, Piece_t origin) const;

    void changeConstrainsCount(Piece_t piece, int delta);

    inline Piece_t rotatePieceCounterClockWise(Piece_t piece) const;
};


#endif //PUZZLE_ROTATABLEPIECEMANAGER_H
