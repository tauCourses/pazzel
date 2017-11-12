//
// Created by private on 11/11/17.
//

#ifndef PUZZLE_PARSER_H
#define PUZZLE_PARSER_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include "PuzzlePiece.h"
#include "string.h"

#define BUFFER_SIZE 1024


using namespace std;

class ParsingErrors {
public:
    bool failedToOpenFile = false;
    vector<int> missingPuzzleElements;
    vector<int> wrongPiecesIds;
    vector<int> wrongPieceFormat;
    vector<string> wrongPieceFormatLine;

    bool hasError();
};


class ParsedPuzzle {
public:
    PuzzlePiece **pieces = nullptr;
    int numberOfPieces = 0;
    ParsingErrors parsingErrors;

    explicit ParsedPuzzle(const char *inputFile);

    ~ParsedPuzzle();
};

#endif //PUZZLE_PARSER_H
