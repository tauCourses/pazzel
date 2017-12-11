#ifndef PUZZLE_PUZZLEPARSER_H
#define PUZZLE_PUZZLEPARSER_H


#include "AbstractPieceManager.h"
#include <iostream>
#include <fstream>

using namespace std;

class PuzzleParser {
    class ParsingErrors {
    public:
        bool failedToOpenFile = false;
        bool numberOfPiecesNotValid = false;
        /*  vector<int> missingPuzzleElements;
          vector<int> wrongPiecesIds;
          vector<string> wrongPieceFormatLine;
          vector<string> notIntegerIds;*/

        bool hasError();
    };

public:
    explicit PuzzleParser(ifstream fin, AbstractPieceManager pieceManager);
    bool hasErrors();
    void exportErrors(ofstream outf);
};



#endif //PUZZLE_PUZZLEPARSER_H
