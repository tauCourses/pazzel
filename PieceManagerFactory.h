#ifndef PUZZLE_PIECEMANAGERFACTORY_H
#define PUZZLE_PIECEMANAGERFACTORY_H

#include <memory>

#include "AbstractPieceManager.h"
#include "BasicPieceManager.h"
#include "RotatablePieceManager.h"

using namespace std;


class PieceManagerFactory {
public:
    static unique_ptr<AbstractPieceManager> getPieceManager(bool enableRotate);
};


#endif //PUZZLE_PIECEMANAGERFACTORY_H
