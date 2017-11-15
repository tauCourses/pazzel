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

    bool parseId(ParsedPuzzle *pp, char* num, int *id)
    {
        try
        {
            *id = std::stoi(num);
            if(*id<1 || *id> pp->numberOfPieces)
            {
                pp->parsingErrors.wrongPiecesIds.push_back(*id);
                return false;
            }
        }
        catch(...)
        {
            pp->parsingErrors.notInteregerIds.emplace_back(num);
            return false;
        }
        return true;
    }

    bool parseSideValue(ParsedPuzzle *pp, char* num, int *val)
    {
        printf("num - %d\n", *num);
        try
        {
            *val = std::stoi(num);
            if(*val<-1 || *val>1)
                return false;
        }
        catch(int e)
        {
            return false;
        }
        return true;
    }

    void parseAllThePiecesFromFile(ParsedPuzzle *pp, FILE *file) {
        char line[BUFFER_SIZE];
        char copy[BUFFER_SIZE];
        char toNumber[BUFFER_SIZE];
        while (fgets(line, BUFFER_SIZE, file)) {
            /* Read puzzle pieces */
            strcpy(copy,line);
            int id, up, down, left, right;
            if(!parseId(pp,strtok(line," "),&id))
                continue;
            if(!parseSideValue(pp,strtok(NULL," "),&left) ||
               !parseSideValue(pp,strtok(NULL," "),&up) ||
               !parseSideValue(pp,strtok(NULL," "),&right) ||
                !parseSideValue(pp,strtok(NULL," "),&down))
            {
                sprintf(toNumber,"%d",id);
                pp->parsingErrors.wrongPieceFormatLine.emplace_back(toNumber);
                pp->parsingErrors.wrongPieceFormatLine.emplace_back(copy);
                pp->pieces[id - 1] = new PuzzlePiece(id, 0, 0, 0, 0);
            }
            else
            {
                if (pp->pieces[id - 1] != nullptr) {
                    delete pp->pieces[id - 1];
                }
                // notice we convert -1,0,1 -> 1,2,3
                pp->pieces[id - 1] = new PuzzlePiece(id, left + 1, up + 1, right + 1, down + 1);
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
}

bool ParsingErrors::hasError() {
    return failedToOpenFile || !missingPuzzleElements.empty() || !wrongPiecesIds.empty() ||
           !notInteregerIds.empty();
}