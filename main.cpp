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
    auto pieceManager = PieceManagerFactory::getPieceManager(cmd.isRotateEnable()); //factory to get piece manager
    PuzzleParser parser(cmd.inputStream, pieceManager); //parse input file and inject pieces into piece Manager
    if (parser.hasErrors()) {//handle parser errors
        parser.exportErrors(cmd.outputStream);
        return -1;
    }
    if (pieceManager->hasErrors()) { //handle piece manager errors:
        pieceManager->exportErrors(cmd.outputStream);
        return -1;
    }
    PuzzleSolver puzzleSolver(pieceManager, cmd.getNumberOfThreads());
    if (puzzleSolver.trySolve()) //return true if succeeded
        puzzleSolver.exportSolution(cmd.outputStream);
    else
        puzzleSolver.exportErrors(cmd.outputStream);
    return 0;
}