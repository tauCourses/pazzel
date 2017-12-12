#include <algorithm>
#include <regex>
#include "PuzzleParser.h"

PuzzleParser::PuzzleParser(ifstream& fin, const unique_ptr<AbstractPieceManager> const &pieceManager) : fin(fin){
    string line;
    if(!this->tryReadFirstLine())
        return ;
    if(this->inValidNumberOfPieces)
        return;
    pieceManager->setSize(this->numberOfPieces);

    while(!fin.eof()) {
        getline(fin, line);
        if(std::all_of(line.begin(), line.end(), isspace))
            continue;
        int id = getPieceId(line); //check if it's a valid id, return -1 if not
        if(id < 0)
            continue;
        pieceManager->addPiece(getNextPiece(id, line));
    }

    checkForMissingParts(pieceManager);
}

bool PuzzleParser::tryReadFirstLine() {
    this->fin >> std::skipws >> "NumElement" >> "=" >> &this->numberOfPieces >> endl;
    if(this->fin.fail())
    {
        this->firstLineMalformed = true;
        return false;
    }
    if(this->numberOfPieces<0)
    {
        this->inValidNumberOfPieces = true;
        return false;
    }
    this->fin >> std::noskipws;
    return true;
}


int PuzzleParser::getPieceId(string &line) {
    auto first_token = line.substr(0, line.find(" \t"));
    if(!this->isInteger(first_token))
    {
        this->notIntegerIds.emplace_back(unique_ptr<string>(first_token));
        return -1;
    }
    int id =  std::stoi(first_token);
    if(id<1 || id > this->numberOfPieces)
    {
        this->wrongPiecesIds.emplace_back(id);
    }
    return -1;

}

unique_ptr<PuzzlePiece> &&PuzzleParser::getNextPiece(int id, string &line) {
    int left, up, right, down;
    line = line.substr(line.find(" \t\r") + 1); //after id
    string rest = string(line);
    if(!tryReadSide(rest, &left) ||
        !tryReadSide(rest, &up) ||
        !tryReadSide(rest, &right) ||
        !tryReadSide(rest, &down) ||
        !std::all_of(rest.begin(), rest.end(), isspace))
    {
        this->wrongPieceFormatLine.emplace_back(make_tuple(unique_ptr<string>(line), id));
        return unique_ptr(PuzzlePiece(id,0,0,0,0));
    }
    return unique_ptr(PuzzlePiece(id,left,up,right,down));
}


bool PuzzleParser::tryReadSide(string &rest, int *side) {
    auto first_token = rest.substr(0, rest.find(" \t"));
    if(!this->isInteger(first_token))
        return false;
    *side =  std::stoi(first_token);
    rest = rest.substr(rest.find(" \t\r") + 1);

    return *side >= -1 && *side <= 1;
}

bool PuzzleParser::isInteger(const string &str) {
    static std::regex isNumber("(\\+|-)?[[:digit:]]+");
    return std::regex_match(str, isNumber);
}

void PuzzleParser::checkForMissingParts(const unique_ptr<AbstractPieceManager> const &pieceManager) {
    auto pieces = pieceManager->getAllPuzzlePieces();
    auto pieceIter = pieces.begin();
    int i;
    for(i=0;i<this->numberOfPieces ;i++)
    {
        if(pieceIter->index != i)
            this->missingPuzzleElements.emplace_back(i);
        else
            pieceIter++;
    }
    for(;i<this->numberOfPieces;i++)
        this->missingPuzzleElements.emplace_back(i);
}

bool PuzzleParser::hasErrors() const{
    return this->firstLineMalformed ||
           this->inValidNumberOfPieces ||
           !this->missingPuzzleElements.empty() ||
           !this->wrongPiecesIds.empty() ||
           !this->wrongPieceFormatLine.empty() ||
           !this->notIntegerIds.empty();
}

void PuzzleParser::exportErrors(ofstream& outf) const{
    if (this->firstLineMalformed) {
        outf << "First line melformed" << "\n";
        return;
    }
    if (this->inValidNumberOfPieces) {
        outf << "Invalid number of elements" << "\n";
        return;
    }
    this->printMissingElements(outf);
    this->printOutOfRangeElements(outf);
    this->printWrongFormatPieces(outf);
    this->printNotValidIdsElements(outf);
}

void PuzzleParser::printMissingElements(ofstream& outf) const{
    if (this->missingPuzzleElements.empty())
        return;
    outf << "Missing puzzle element(s) with the following IDs:";
    bool firstElement = true;
    for (auto missingElement : this->missingPuzzleElements) {
        if(firstElement) {
            outf << " " << missingElement;
            firstElement = false;
        }else
            outf << ", " << missingElement;
    }
    outf << "\n";
}

void PuzzleParser::printOutOfRangeElements(ofstream& outf) const{
    if (this->wrongPiecesIds.empty())
        return;

    outf << "Puzzle of size " << this->numberOfPieces << " cannot have the following IDs:";
    bool firstElement = true;

    for (auto outOfRangeElement : this->wrongPiecesIds) {
        if(firstElement) {
            outf << " " << outOfRangeElement;
            firstElement = false;
        }else
            outf << ", " << outOfRangeElement;
    }
    outf << "\n";
}

void PuzzleParser::printWrongFormatPieces(ofstream& outf) const{
    if (this->wrongPieceFormatLine.empty())
        return;

    for (auto tuple : this->wrongPieceFormatLine) {
        outf << "Puzzle ID " << get<0>(tuple);
        outf << " has wrong data:  " << get<1>(tuple) << "\n";
    }
}

void PuzzleParser::printNotValidIdsElements(ofstream& outf) const{
    if (this->notIntegerIds.empty())
        return;

    outf << "The following element(s) doesn't has a valid id:";
    bool firstElement = true;

    for (const auto const &id : this->notIntegerIds) {
        if(firstElement) {
            outf << " " << id;
            firstElement = false;
        }else
            outf << ", " << id;
    }
}