#ifndef PUZZLE_ROTATABLEPIECEMANAGER_H
#define PUZZLE_ROTATABLEPIECEMANAGER_H

#include <algorithm>
#include "AbstractPieceManager.h"
#include "PuzzlePieceConstrain.h"

class RotatablePieceManager : public AbstractPieceManager {
public:
    RotatablePieceManager();

    vector<AbstractPieceManager::Shape> getAllPossiblePuzzleShapes() const override;

    int numOfOptionsForConstrain(const PieceRepository &pieceRepository, Piece_t constrain) const override;

    void addPiece(PieceRepository &pieceRepository, unique_ptr<PuzzlePiece> piece) override;

    void printPiece(Piece_t piece, ofstream &out) override;

protected:
    void addPieceToRepository(PieceRepository &pieceRepository, Piece_t piece) const override;

    void removePieceFromRepository(PieceRepository &pieceRepository, Piece_t piece) const override;

    bool pieceExistInRepository(const PieceRepository &pieceRepository, Piece_t piece) const override;

    //error hundeling:
    bool hasAllCorners(const PieceRepository &pieceRepository) const override;

    bool hasTwoSideForARaw() const;

    void printMissingCorners(const PieceRepository &pieceRepository, ofstream &fout) const override;

private:
    static Piece_t lookupTable[numberOfConstrains]; //translate Piece_t to generic Permutation

    bool isPuzzleShapePossible(AbstractPieceManager::Shape shape) const override;

    int numberOfCorners() const;

    void initialLookupTable() const;

    bool isPermutation(Piece_t p1, Piece_t p2) const;

    int getPermutationDegree(Piece_t current, Piece_t origin) const;

    void changeConstrainsCount(PieceRepository &pieceRepository, Piece_t piece, int delta) const;

    inline Piece_t rotatePieceCounterClockWise(Piece_t piece) const;

};


#endif //PUZZLE_ROTATABLEPIECEMANAGER_H
