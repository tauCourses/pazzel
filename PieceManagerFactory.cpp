#include "PieceManagerFactory.h"

//a factory to return a unique_ptr of the right prototypePieceManager.
unique_ptr<AbstractPieceManager> PieceManagerFactory::getPieceManager(bool enableRotate) {
    if (enableRotate) {
        return unique_ptr<AbstractPieceManager>(new RotatablePieceManager);
    }
    return unique_ptr<AbstractPieceManager>(new BasicPieceManager);
}
