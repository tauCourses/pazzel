#ifndef PUZZLE_BASICPIECEMANAGER_H
#define PUZZLE_BASICPIECEMANAGER_H

#include "AbstractPieceManager.h"
#include "PuzzlePieceConstrain.h"
#include <memory>

class BasicPieceManager : public AbstractPieceManager {
public:
    vector<AbstractPieceManager::Shape> getAllPossiblePuzzleShapes() const override;

    int numOfOptionsForConstrain(const PieceRepository &pieceRepository, Piece_t constrain) const override;

    void addPiece(PieceRepository &pieceRepository, unique_ptr<PuzzlePiece> piece) override;

    bool preConditions() const override;

    BasicPieceManager() = default;

protected:
    void addPieceToRepository(PieceRepository &pieceRepository, Piece_t piece) const override;

    void removePieceFromRepository(PieceRepository &pieceRepository, Piece_t piece) const override;

    bool pieceExistInRepository(const PieceRepository &pieceRepository, Piece_t piece) const override;

private:
    BasicPieceManager(BasicPieceManager const &copyPieceManager);

    bool isPuzzleShapePossible(AbstractPieceManager::Shape shape) const override;

    //error hundeling:
    bool hasAllCorners(const PieceRepository &pieceRepository) const override;

    void printMissingCorners(const PieceRepository &pieceRepository, ofstream &fout) const override;

    void addPieceToConstrain(PieceRepository &pieceRepository, Piece_t piece) const;

    inline void removePieceFromConstrain(PieceRepository &pieceRepository, Piece_t piece) const;

    void printPiece(Piece_t piece, ofstream &out) override;


};


#endif //PUZZLE_BASICPIECEMANAGER_H
