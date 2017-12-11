//
// Created by private on 12/11/17.
//

#ifndef PUZZLE_PARSER_H
#define PUZZLE_PARSER_H


class Parser {

private:
    class ParsingErrors {
    public:
        bool failedToOpenFile = false;
        bool numberOfPiecesNotValid = false;
        vector<int> missingPuzzleElements;
        vector<int> wrongPiecesIds;
        vector<string> wrongPieceFormatLine;
        vector<string> notIntegerIds;

        bool hasError();
    };

public:
    vector<PuzzlePiece> getPieces(char* fileName);
};


#endif //PUZZLE_PARSER_H
