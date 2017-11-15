//
// Created by private on 11/11/17.
//
#include "ParsedPuzzle.h"

namespace {
    int getNumberOfPieces(FILE *file) {
		char line[BUFFER_SIZE] = { 0 };
		char buffer[BUFFER_SIZE] = { 0 };
        int i = -1;
        if (fgets(line, BUFFER_SIZE, file)) { // if fail -> empty file.
            sscanf(line, "NumElements = %d%s", &i,buffer);
            if(strlen(buffer)!=0)
                i=-1;
        }
        return i;
    }

    bool isNumber(char* num)
    {
        if(*num == '-')
            num++;
        for(;*num;num++)
            if(!isdigit(*num) && *num != '\n' && *num != '\r')
                return false;
        return true;
    }
    bool parseId(ParsedPuzzle *pp, char* num, int *id)
    {

        if(!isNumber(num)) {
            pp->parsingErrors.notIntegerIds.emplace_back(num);
            return false;
        }

        *id = std::stoi(num);
        if(*id<1 || *id> pp->numberOfPieces)
        {
            pp->parsingErrors.wrongPiecesIds.push_back(*id);
            return false;
        }

        return true;
    }

    bool parseSideValue(int *val)
    {
        char* num = strtok(nullptr," ");
        if(num == nullptr)
            return false;

        if(!isNumber(num))
            return false;

        *val = std::stoi(num);
        return !(*val < -1 || *val > 1);

    }

    int is_empty(const char *s) {
        while (*s != '\0') {
            if (!isspace((unsigned char)*s))
                return 0;
            s++;
        }
        return 1;
    }

    void parseAllThePiecesFromFile(ParsedPuzzle *pp, FILE *file) {
		char line[BUFFER_SIZE] = { 0 };
        char copy[BUFFER_SIZE] = { 0 };
        char toNumber[BUFFER_SIZE] = { 0 };
        while (fgets(line, BUFFER_SIZE, file)) {
            if (is_empty(line))
                continue;
            line[strcspn(line, "\r\n")] = 0;
            strcpy(copy,line);
            int id, up, down, left, right;
            if(!parseId(pp,strtok(line," "),&id))
                continue;
            if(!parseSideValue(&left) ||
               !parseSideValue(&up) ||
               !parseSideValue(&right) ||
               !parseSideValue(&down) ||
               strtok(nullptr," ") != nullptr)
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
            pp->parsingErrors.numberOfPiecesNotValid = true;
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
    return failedToOpenFile || numberOfPiecesNotValid ||
            !missingPuzzleElements.empty() || !wrongPiecesIds.empty() ||
            !wrongPieceFormatLine.empty() || !notIntegerIds.empty();
}