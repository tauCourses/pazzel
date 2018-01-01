#ifndef PUZZLE_BASICPIECEMANAGER_H
#define PUZZLE_BASICPIECEMANAGER_H

#include "AbstractPieceManager.h"
#include "PuzzlePieceConstrain.h"
#include <memory>

class BasicPieceManager : public AbstractPieceManager {
public:
    vector<AbstractPieceManager::Shape> getAllPossiblePuzzleShapes() const override;

    int numOfOptionsForConstrain(Piece_t constrain) const override;

    void addPiece(unique_ptr<PuzzlePiece> piece) override;

    bool preConditions() const override;

    BasicPieceManager() = default;

    unique_ptr<AbstractPieceManager> clone() const override;

protected:
    void addPieceToRepository(Piece_t piece) override;

    void removePieceFromRepository(Piece_t piece) override;

    bool pieceExistInRepository(Piece_t piece) const override;

private:
    BasicPieceManager(BasicPieceManager const &copyPieceManager);

    bool isPuzzleShapePossible(AbstractPieceManager::Shape shape) const override;

    //error handeling:
    bool hasAllCorners() const override;

    void printMissingCorners(ofstream &fout) const override;

    void addPieceToConstrain(Piece_t piece);

    void removePieceFromConstrain(Piece_t piece);

    void printPiece(Piece_t piece, ofstream &out) override;
};

#endif //PUZZLE_BASICPIECEMANAGER_H
