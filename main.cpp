#include "CommandLineManager.h"
#include "PieceManagerFactory.h"
#include "PuzzleSolver.h"
#include "PuzzleParser.h"

int main(int argc, char **argv) {
    CommandLineManager cmd(argc, argv); //parse the command line arguments
    if (cmd.hasErrors()) {
        cmd.exportErrors();
        return -1;
    }

    //factory to get piece manager
    auto pieceManager = PieceManagerFactory::getPieceManager(cmd.isRotateEnable());
    AbstractPieceManager::PieceRepository prototypePiecesRepository = {};

    //parse input file and inject pieces into piece Manager
    PuzzleParser parser(cmd.inputStream, pieceManager, prototypePiecesRepository);
    if (parser.hasErrors()) {//handle parser errors
        parser.exportErrors(cmd.outputStream);
        return -1;
    }
    if (pieceManager->hasErrors(prototypePiecesRepository)) { //handle piece manager errors:
        pieceManager->exportErrors(prototypePiecesRepository, cmd.outputStream);
        return -1;
    }

    PuzzleSolver puzzleSolver(pieceManager, prototypePiecesRepository, cmd.getNumberOfThreads());
    if (puzzleSolver.trySolve()) //return true if succeeded
        puzzleSolver.exportSolution(cmd.outputStream);
    else
        puzzleSolver.exportErrors(cmd.outputStream);
    return 0;
}