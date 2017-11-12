//
// Created by private on 11/11/17.
//
#include "ParsedPuzzle.h"


ParsedPuzzle::ParsedPuzzle(const char *inputFile) {

    FILE *file;
    char line[BUFFER_SIZE];

    file = fopen(inputFile, "r");
    if (!file) {
        parsingErrors.failedToOpenFile = true;
        return;
    }

    if (fgets(line, BUFFER_SIZE, file) && // if fail -> empty file.
        sscanf(line, "NumElements = %d", &numberOfPieces)) {
        pieces = new PuzzlePiece *[numberOfPieces]();
    } else {
        // Wrong input file.
        parsingErrors.failedToOpenFile = true;
        return;
    }

    while (fgets(line, BUFFER_SIZE, file)) {
        /* Read puzzle pieces */
        int id, up, down, left, right, checker, lengthRead;
        bool isUsable = true;
        if ((lengthRead = sscanf(line, " %d %d %d %d %d %d",
                                 &id, &left, &up, &right, &down, &checker)) != 0) { // 0 = no ID -> emptyLine
            if (lengthRead != 5) { // wrong format
                parsingErrors.wrongPieceFormat.push_back(id);
                parsingErrors.wrongPieceFormatLine.emplace_back(line);
                pieces[id - 1] = new PuzzlePiece(id, 0, 0, 0, 0);
                isUsable = false;
            }
            if (id < 1 || id > numberOfPieces) { // wrong id
                parsingErrors.wrongPiecesIds.push_back(id);
                isUsable = false;
            }
            if (isUsable) {
                if (pieces[id - 1] != nullptr) {
                    delete pieces[id - 1];
                }
                // notice we convert -1,0,1 -> 1,2,3
                pieces[id - 1] = new PuzzlePiece(id, left + 2, up + 2, right + 2, down + 2);
            }
        }
    }

    if (!feof(file)) {
        // Something went wrong when reading the file
        parsingErrors.failedToOpenFile = true;
    }
    for (int i = 0; i < numberOfPieces; ++i) {
        if (!pieces[i]) parsingErrors.missingPuzzleElements.push_back(i + 1);
    }
    fclose(file);
}

ParsedPuzzle::~ParsedPuzzle() {
    if (!pieces) {
        for (int i = 0; i < numberOfPieces; ++i) if (pieces[i] != nullptr) delete pieces[i];
        if (pieces != nullptr) delete[] pieces;
    }
};

bool ParsingErrors::hasError() {
    return failedToOpenFile || !missingPuzzleElements.empty() || !wrongPiecesIds.empty() ||
           !wrongPieceFormat.empty();
}