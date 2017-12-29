#ifndef PUZZLE_BASICPIECEMANAGER_H
#define PUZZLE_BASICPIECEMANAGER_H

#include "AbstractPieceManager.h"
#include "PuzzlePieceConstrain.h"
#include <memory>

class BasicPieceManager : public AbstractPieceManager {
public:
    vector<Shape> getAllPossiblePuzzleShapes() override;

    int numOfOptionsForConstrain(Piece_t constrain) override;

    void addPiece(unique_ptr<PuzzlePiece> piece) override;

    bool preConditions() override;

    unique_ptr<AbstractPieceManager> clone() override;

    BasicPieceManager () = default;

protected:
    void addPieceToRepository(Piece_t piece) override;

    void removePieceFromRepository(Piece_t piece) override;

    bool pieceExistInRepository(Piece_t piece) override;

private:
    BasicPieceManager(BasicPieceManager const & copyPieceManager);
    int constrainOption[numberOfConstrains] = {0};

    bool isPuzzleShapePossible(Shape shape) override;

    //error hundeling:
    bool hasAllCorners() override;

    void printMissingCorners(ofstream &fout) override;

    void addPieceToOption(Piece_t piece);

    inline void removePieceFromOption(Piece_t piece);

    void printPiece(Piece_t piece, ofstream &out) override;


};


#endif //PUZZLE_BASICPIECEMANAGER_H
