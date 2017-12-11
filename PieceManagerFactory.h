#ifndef PUZZLE_PIECEMANAGERFACTORY_H
#define PUZZLE_PIECEMANAGERFACTORY_H

#include "AbstractPieceManager.h"

class PieceManagerFactory {
public:
    static AbstractPieceManager getPieceManager(bool enableRotate);
};


#endif //PUZZLE_PIECEMANAGERFACTORY_H
