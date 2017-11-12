//
// Created by private on 11/11/17.
//

#ifndef PAZZEL_EXPORTER_H
#define PAZZEL_EXPORTER_H

#include <iostream>
#include "ParsedPuzzle.h"
#include "PuzzleSolver.h"

using namespace std;

namespace exporter {
    void exportPuzzleParsingErrors(const char *outputFile, ParsedPuzzle &puzzle);

    void exportPuzzleSolvingErrors(const char *outputFile, SolverErrors &solverErrors);

    void exportSolution(const char *outputFile, ParsedPuzzle &puzzle, PuzzleSolution *puzzleSolution);
};


#endif //PAZZEL_EXPORTER_H
