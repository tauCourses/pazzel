#ifndef PUZZLE_EXPORTER_H
#define PUZZLE_EXPORTER_H

#include <iostream>
#include "ParsedPuzzle.h"
#include "PuzzleSolver.h"

using namespace std;

namespace exporter {
    void exportPuzzleParsingErrors(const char *outputFile, ParsedPuzzle &puzzle);

    void exportPuzzleSolvingErrors(const char *outputFile, SolverErrors &solverErrors);

    void exportSolution(const char *outputFile, ParsedPuzzle &puzzle, PuzzleSolution *puzzleSolution);
}


#endif //PUZZLE_EXPORTER_H
