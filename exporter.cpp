//
// Created by private on 11/11/17.
//

#include "exporter.h"

/*!
 * Helper Functions
 */
namespace {
    void printMissingElements(FILE *file, vector<int> missingElements) {
        if (missingElements.empty()) return;
        fprintf(file, "%s", "Missing puzzle element(s) with the following IDs:");
        bool firstElement = true;
        for (int missingElement : missingElements) {
            fprintf(file,
                    (firstElement) ? " %d" : ", %d",
                    missingElement);
            firstElement = true;
        }
        fprintf(file, "\n");
    }

    void printOutOfRangeElements(FILE *file, vector<int> outOfRangeElements, int numberOfElements) {
        if (outOfRangeElements.empty()) return;
        fprintf(file, "Puzzle of size %d cannot have the following IDs:", numberOfElements);
        bool firstElement = true;

        for (int outOfRangeElement : outOfRangeElements) {
            fprintf(file,
                    (firstElement) ? " %d" : ", %d",
                    outOfRangeElement);
            firstElement = true;
        }
        fprintf(file, "\n");
    }

    void printWrongFormatPieces(FILE *file, vector<int> wrongFormatPieces, vector<string> wrongFormatLines) {
        if (wrongFormatPieces.empty()) return;
        auto wrongFormatPiece = wrongFormatPieces.begin();
        auto wrongFormatLine = wrongFormatLines.begin();
        for (; wrongFormatPiece != wrongFormatPieces.end() && wrongFormatLine != wrongFormatLines.end();
               ++wrongFormatPiece, ++wrongFormatLine) {
            fprintf(file, "Puzzle ID %d has wrong data: %s", *wrongFormatPiece, (*wrongFormatLine).c_str());
            if ((*wrongFormatLine)[wrongFormatLine->length() - 1] != '\n') {
                fprintf(file, "\n");
            }
        }
    }
}

void exporter::exportPuzzleParsingErrors(const char *outputFile, ParsedPuzzle &puzzle) {
    FILE *file = fopen(outputFile, "w");
    if (!file) {
        std::cerr << "could not open output file. :" << outputFile << std::endl;
    } else {
        if (puzzle.parsingErrors.failedToOpenFile) {
            fprintf(file, "Failed to open input file.\n");
            return;
        }
        printMissingElements(file, puzzle.parsingErrors.missingPuzzleElements);
        printOutOfRangeElements(file, puzzle.parsingErrors.wrongPiecesIds, puzzle.numberOfPieces);
        printWrongFormatPieces(file, puzzle.parsingErrors.wrongPieceFormat,
                               puzzle.parsingErrors.wrongPieceFormatLine);

        fclose(file);
    }
}

void exporter::exportPuzzleSolvingErrors(const char *outputFile, SolverErrors &solverErrors) {
    FILE *file = fopen(outputFile, "w");
    if (!file) {
        std::cerr << "could not open output file. :" << outputFile << std::endl;
    } else {
        if (solverErrors.wrongNumberOfStraightLine) {
            fprintf(file, "Cannot solve puzzle: wrong number of straight edges\n");
        }
        if (solverErrors.missingTL) {
            fprintf(file, "Cannot solve puzzle: missing corner element: TL\n");
        }
        if (solverErrors.missingTR) {
            fprintf(file, "Cannot solve puzzle: missing corner element: TR\n");
        }
        if (solverErrors.missingBL) {
            fprintf(file, "Cannot solve puzzle: missing corner element: BL\n");
        }
        if (solverErrors.missingBR) {
            fprintf(file, "Cannot solve puzzle: missing corner element: BR\n");
        }
        if (solverErrors.sumOfEdgesIsNotZero) {
            fprintf(file, "Cannot solve puzzle: sum of edges is not zero\n");
        }
        if (solverErrors.couldNotSolvePuzzle) {
            fprintf(file, "Cannot solve puzzle: it seems that there is no proper solution\n");
        }

        fclose(file);
    }
}


void exporter::exportSolution(const char *outputFile, ParsedPuzzle &puzzle, PuzzleSolution *puzzleSolution) {
    FILE *file = fopen(outputFile, "w");
    if (!file) {
        std::cerr << "could not open output file. :" << outputFile << std::endl;
    } else {
        vector<vector<int> > vec(1 << 8, vector<int>(0));
        for (int i = 0; i < puzzle.numberOfPieces; i++) {
            auto piece = puzzle.pieces[i];
            vec[piece->representor()].push_back(piece->index);
        }
        for (int i = 0; i < puzzleSolution->row; ++i) {
            bool firstInLine = true;
            for (int j = 0; j < puzzleSolution->col; ++j) {
                fprintf(file, (firstInLine) ? "%d" : " %d", vec[puzzleSolution->get(i, j)].back());
                vec[puzzleSolution->get(i, j)].pop_back();
                firstInLine = false;
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }
}
