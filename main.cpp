#include "CommandLineManager.h"
#include "PieceManagerFactory.h"
#include "PuzzleSolver.h"
#include "PuzzleParser.h"

int main(int argc, char **argv) {
    CommandLineManager cmd(argc, argv); //parse the command line arguments
    if (cmd.hasErrors())
        return -1;
    try{
        //factory to get piece manager
        auto pieceManager = PieceManagerFactory::getPieceManager(cmd.isRotateEnable());
        PuzzleParser parser(cmd.inputStream);
        parser.injectPieces(pieceManager); //parse file and inject pieces into pieceManager
        PuzzleSolver puzzleSolver(pieceManager, cmd.getNumberOfThreads());
        puzzleSolver.solve();
        puzzleSolver.exportSolution(cmd.outputStream);
    } catch (PuzzleException& e) {
        cmd.outputStream << e.errorMessage;
        return -1;
    }
    return 0;
}