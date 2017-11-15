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
            firstElement = false;
        }
    }

    void printOutOfRangeElements(FILE *file, vector<int> outOfRangeElements, int numberOfElements) {
        if (outOfRangeElements.empty()) return;
        if(ftell(file) != 0)
            fprintf(file,"\n");
        fprintf(file, "Puzzle of size %d cannot have the following IDs:", numberOfElements);
        bool firstElement = true;

        for (int outOfRangeElement : outOfRangeElements) {
            fprintf(file,
                    (firstElement) ? " %d" : ", %d",
                    outOfRangeElement);
            firstElement = false;
        }
    }

    void notValidIdsElements(FILE *file, vector<string> notIntegerIds) {
        if (notIntegerIds.empty()) return;
        if(ftell(file) != 0)
            fprintf(file,"\n");
        fprintf(file, "The following element(s) doesn't has a valid id:");
        bool firstElement = true;

        for (string &notIntegerId : notIntegerIds) {
            fprintf(file,
                    (firstElement) ? " %s" : ", %s",
                    notIntegerId.c_str());
            firstElement = false;
        }
    }

    void printWrongFormatPieces(FILE *file, vector<string> wrongFormatLines) {
        if (wrongFormatLines.empty()) return;

        auto wrongFormatLine = wrongFormatLines.begin();
        while(wrongFormatLine != wrongFormatLines.end())
        {
            if(ftell(file) != 0)
                fprintf(file,"\n");
            fprintf(file, "Puzzle ID %s has wrong data: %s", (*wrongFormatLine).c_str(), (*(wrongFormatLine+1)).c_str());
            wrongFormatLine+=2;
        }
    }

    void printToFile(FILE* file, const char* str)  {
        if(ftell(file) != 0)
            fprintf(file,"\n");
        fprintf(file, "%s", str);
    }
}

void exporter::exportPuzzleParsingErrors(const char *outputFile, ParsedPuzzle &puzzle) {
    if (puzzle.parsingErrors.failedToOpenFile) {
        std::cerr << "[USAGE] input file doesn't exist" << std::endl;
        return;
    }
    FILE *file = fopen(outputFile, "w");
    if (!file) {
        std::cerr << "could not open output file: " << outputFile << std::endl;
    } else {
        if (puzzle.parsingErrors.numberOfPiecesNotValid) {
            fprintf(file, "invalid number of elements");
            return;
        }
        printMissingElements(file, puzzle.parsingErrors.missingPuzzleElements);
        printOutOfRangeElements(file, puzzle.parsingErrors.wrongPiecesIds, puzzle.numberOfPieces);
        printWrongFormatPieces(file, puzzle.parsingErrors.wrongPieceFormatLine);
        notValidIdsElements(file, puzzle.parsingErrors.notIntegerIds);

        fclose(file);
    }
}
void exporter::exportPuzzleSolvingErrors(const char *outputFile, SolverErrors &solverErrors) {
    FILE *file = fopen(outputFile, "w");
    if (!file) {
        std::cerr << "could not open output file. :" << outputFile << std::endl;
    } else {
        if (solverErrors.wrongNumberOfStraightLine)
            printToFile(file, "Cannot solve puzzle: wrong number of straight edges");

        if (solverErrors.missingTL)
            printToFile(file, "Cannot solve puzzle: missing corner element: TL");

        if (solverErrors.missingTR)
            printToFile(file, "Cannot solve puzzle: missing corner element: TR");

        if (solverErrors.missingBL)
            printToFile(file, "Cannot solve puzzle: missing corner element: BL");

        if (solverErrors.missingBR)
            printToFile(file, "Cannot solve puzzle: missing corner element: BR");

        if (solverErrors.sumOfEdgesIsNotZero)
            printToFile(file, "Cannot solve puzzle: sum of edges is not zero");

        if (solverErrors.couldNotSolvePuzzle)
            printToFile(file, "Cannot solve puzzle: it seems that there is no proper solution");


        fclose(file);
    }
}


void exporter::exportSolution(const char *outputFile, ParsedPuzzle &puzzle, PuzzleSolution *puzzleSolution) {
    FILE *file = fopen(outputFile, "w");
    if (!file)
        std::cerr << "could not open output file. :" << outputFile << std::endl;
    else {
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
