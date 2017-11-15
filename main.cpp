#include "ParsedPuzzle.h"
#include "PuzzleSolver.h"
#include "exporter.h"

using namespace exporter;

int main(int argc, char **argv) {
   /* if (argc != 3) {
        std::cerr << "[USAGE] input_puzzle_file_path output_result_file_path" << std::endl;
        return -1;
    }*/
	char *inputFilePath = "C:\\Users\\t-idkess\\CLionProjects\\puzzle\\a",
		*outputFilePath = "C:\\Users\\t-idkess\\CLionProjects\\puzzle\\b";

    ParsedPuzzle puzzle = ParsedPuzzle(inputFilePath);
    if (puzzle.parsingErrors.hasError()) {
        //  export error to output file
        exportPuzzleParsingErrors(outputFilePath, puzzle);
        return -1;
    }

    PuzzleSolver puzzleSolver = PuzzleSolver(puzzle);
    if (puzzleSolver.solverErrors.hasError()) {
        exportPuzzleSolvingErrors(outputFilePath, puzzleSolver.solverErrors);
        return -1;
    } else {
        exportSolution(outputFilePath, puzzle, puzzleSolver.puzzleSolution);
    }
    return 0;
}