#ifndef PUZZLE_PUZZLEPARSER_H
#define PUZZLE_PUZZLEPARSER_H

#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>
#include <regex>

#include "AbstractPieceManager.h"
#include "PuzzlePiece.h"

using namespace std;

class PuzzleParser {
private:
    ifstream &fin;

    //errors types:
    int numberOfPieces;
    bool inValidNumberOfPieces = false;
    vector<int> missingPuzzleElements;
    vector<int> elementsAppearMoreThanOnce; //elements that have more than one occurrence
    vector<int> wrongPiecesIds;
    vector<tuple<int, string>> wrongPieceFormatLine;
    vector<string> notIntegerIds;

    bool tryReadFirstLine(); //return false if failed
    int getPieceId(string &line);

    unique_ptr<PuzzlePiece> getNextPiece(int id, string &line);

    bool tryReadSide(string &rest, int &side);

    bool isInteger(const string &str);

    bool isStringEmpty(const string &str);

    void checkForMissingParts(const unique_ptr<AbstractPieceManager> &pieceManager);

    //export errors functions:
    void printMissingElements(ofstream &outf) const;

    void printOutOfRangeElements(ofstream &outf) const;

    void printWrongFormatPieces(ofstream &outf) const;

    void printNotValidIdsElements(ofstream &outf) const;

    void printElementsAppearMoreThanOnce(ofstream &outf) const;

    void throwException() const;

    void exportErrors(ofstream &outf) const;
public:
    explicit PuzzleParser(ifstream &fin);

    void injectPieces(const unique_ptr<AbstractPieceManager> &pieceManager);
};

#endif //PUZZLE_PUZZLEPARSER_H
