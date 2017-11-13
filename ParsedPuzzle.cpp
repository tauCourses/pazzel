//
// Created by private on 11/11/17.
//
#include "ParsedPuzzle.h"

namespace {
    int getNumberOfPieces(FILE *file) {
        char line[BUFFER_SIZE];
        int i = -1;
        if (fgets(line, BUFFER_SIZE, file)) { // if fail -> empty file.
            sscanf(line, "NumElements = %d", &i);
        }
        return i;
    }

    void parseAllThePiecesFromFile(ParsedPuzzle *pp, FILE *file) {
        char line[BUFFER_SIZE];
        while (fgets(line, BUFFER_SIZE, file)) {
            /* Read puzzle pieces */
            int id, up, down, left, right, checker, lengthRead;
            bool isUsable = true;
            if ((lengthRead = sscanf(line, " %d %d %d %d %d %d",
                                     &id, &left, &up, &right, &down, &checker)) != 0) { // 0 = no ID -> emptyLine
                if (lengthRead != 5) { // wrong format
                    pp->parsingErrors.wrongPieceFormat.push_back(id);
                    pp->parsingErrors.wrongPieceFormatLine.emplace_back(line);
                    pp->pieces[id - 1] = new PuzzlePiece(id, 0, 0, 0, 0);
                    isUsable = false;
                }
                if (id < 1 || id > pp->numberOfPieces) { // wrong id
                    pp->parsingErrors.wrongPiecesIds.push_back(id);
                    isUsable = false;
                }
                if (isUsable) {
                    if (pp->pieces[id - 1] != nullptr) {
                        delete pp->pieces[id - 1];
                    }
                    // notice we convert -1,0,1 -> 1,2,3
                    pp->pieces[id - 1] = new PuzzlePiece(id, left + 1, up + 1, right + 1, down + 1);
                }
            }
        }

        if (!feof(file)) {
            // Something went wrong when reading the file
            pp->parsingErrors.failedToOpenFile = true;
        }
    }

    void hasMissingPieces(ParsedPuzzle *pp) {
        for (int i = 0; i < pp->numberOfPieces; ++i) {
            if (!pp->pieces[i]) pp->parsingErrors.missingPuzzleElements.push_back(i + 1);
        }
    }

    void parseFile(ParsedPuzzle *pp, FILE *file) {
        pp->numberOfPieces = getNumberOfPieces(file);
        if (pp->numberOfPieces <= 0) {
            pp->parsingErrors.failedToOpenFile = true;
            return;
        }
        pp->pieces = new PuzzlePiece *[pp->numberOfPieces]();

        parseAllThePiecesFromFile(pp, file);

        hasMissingPieces(pp);
    }
}

ParsedPuzzle::ParsedPuzzle(const char *inputFile) {
    FILE *file;
    file = fopen(inputFile, "r");
    if (!file) {
        parsingErrors.failedToOpenFile = true;
        return;
    }
    parseFile(this, file);
    fclose(file);
}

ParsedPuzzle::~ParsedPuzzle() {
    if (!pieces) {
        for (int i = 0; i < numberOfPieces; ++i) if (pieces[i] != nullptr) delete pieces[i];
        delete[] pieces;
    }
};

bool ParsingErrors::hasError() {
    return failedToOpenFile || !missingPuzzleElements.empty() || !wrongPiecesIds.empty() ||
           !wrongPieceFormat.empty();
}