#include "CommandLineManager.h"
#include "PieceManagerFactory.h"
#include "PuzzleSolver.h"

int main(int argc, char **argv)
{
    auto cmd = CommandLineManager(argc, argv); //parse the command line arguments
    if(cmd.hasErrors())
    {
        cmd.exportError();
        return -1;
    }
    auto pieceManager = PieceManagerFactory(parser, cmd.rotateEnabled); //factory to get the right piece manager
    auto parser = Parser(cmd.inputFile, pieceManager);
    if(parser.hasErrors())
    {
        parser.exportErrors(cmd.outputFile);
        return -1;
    }
    if(pieceManager.hasErrors()) {
        pieceManager.exportErrors(cmd.outputFile);
        return -1;
    }
    auto puzzleSolver = PuzzleSolver(pieceManager);
    if(puzzleSolver.trySolve()) //return true if succeeded
        puzzleSolver.exportSulotion(cmd.outputFile);
    else
        puzzleSolver.exportError(cmd.outputFile);
    return 0;
}