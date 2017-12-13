#include "PieceManagerFactory.h"

unique_ptr<AbstractPieceManager> PieceManagerFactory::getPieceManager(bool enableRotate) {
    if(enableRotate)
        printf("yayy\n");
        //return unique_ptr<AbstractPieceManager>(new RotatablePieceManager);
    return unique_ptr<AbstractPieceManager>(new BasicPieceManager);
}
