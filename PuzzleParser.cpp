#include "PuzzleParser.h"

PuzzleParser::PuzzleParser(ifstream &fin, const unique_ptr<AbstractPieceManager> &pieceManager) : fin(fin) {
    string line;
    if (!this->tryReadFirstLine()) {
        return;
    }
    while (!fin.eof()) {
        getline(fin, line);
        if (this->isStringEmpty(line))
            continue;
        int id = this->getPieceId(line); //check if it's a valid id, return -1 if not
        if (id < 0)
            continue;
        pieceManager->addPiece(getNextPiece(id, line));
    }
    checkForMissingParts(pieceManager);
}

bool PuzzleParser::tryReadFirstLine() {
    string line;
    getline(fin, line);
    auto number_token = line.substr(line.find_first_of('=') + 1);
        if (!this->isInteger(number_token)) {
        this->inValidNumberOfPieces = true;
        return false;
    }
    this->numberOfPieces = std::stoi(number_token);
    if (this->numberOfPieces < 0) {
        this->inValidNumberOfPieces = true;
        return false;
    }
    return true;
}


int PuzzleParser::getPieceId(string &line) {
    auto first_token = line.substr(line.find_first_not_of(" \t")); //ignore spaces before any char
    first_token = first_token.substr(0, first_token.find_first_of(" \t")); //
    if (!this->isInteger(first_token)) {
        this->notIntegerIds.emplace_back(first_token);
        return -1;
    }
    int id = std::stoi(first_token);
    if (id < 1 || id > this->numberOfPieces) {
        this->wrongPiecesIds.emplace_back(id);
        return -1;
    }
    return id;

}

unique_ptr<PuzzlePiece> PuzzleParser::getNextPiece(int id, string &line) {
    int left, up, right, down;
    auto originLine = line;
    string rest = line.substr(line.find_first_not_of(" \t")); //before id
    rest = rest.substr(rest.find_first_of(" \t")); //after id
    bool leftOK = this->tryReadSide(rest, left);
    bool upOK = this->tryReadSide(rest, up);
    bool rightOK = this->tryReadSide(rest, right);
    bool downOK = this->tryReadSide(rest, down);
    bool restOK = this->isStringEmpty(rest);

    if (leftOK && rightOK && downOK && upOK && restOK) {
        return unique_ptr<PuzzlePiece>(new PuzzlePiece(id, left, up, right, down));
    } else {
        string s = rest;
        this->wrongPieceFormatLine.emplace_back(make_tuple(id, originLine));
        return unique_ptr<PuzzlePiece>(new PuzzlePiece(id, 0, 0, 0, 0));
    }
}


bool PuzzleParser::tryReadSide(string &rest, int &side) {
    if (isStringEmpty(rest))
        return false;
    rest = rest.substr(rest.find_first_not_of(" \t"));
    auto first_token = rest.substr(0, rest.find_first_of(" \t"));
    rest = rest.substr(first_token.size());
    if (!this->isInteger(first_token))
        return false;
    side = std::stoi(first_token);
    return side >= -1 && side <= 1;
}

bool PuzzleParser::isStringEmpty(const string &str) {
    static auto isspaceLambda = [](unsigned char const c) { return std::isspace(c); };
    return std::all_of(str.begin(), str.end(), isspaceLambda);

}

bool PuzzleParser::isInteger(const string &str) {
    static std::regex isNumber("^([ \t]*)[-]?(0|[1-9][0-9]*)([ \t]*)$");
    return std::regex_match(str, isNumber);
}

void PuzzleParser::checkForMissingParts(const unique_ptr<AbstractPieceManager> &pieceManager) {
    for (int i = 0; i < this->numberOfPieces; i++) {
        int occurrences = pieceManager->getNumOfOccurrences(i + 1);
        if (occurrences == 0)
            this->missingPuzzleElements.emplace_back(i + 1);
        if (occurrences > 1)
            this->elementsAppearMoreThanOnce.emplace_back(i + 1);
    }

}

bool PuzzleParser::hasErrors() const {
    return this->inValidNumberOfPieces ||
           !this->missingPuzzleElements.empty() ||
           !this->wrongPiecesIds.empty() ||
           !this->wrongPieceFormatLine.empty() ||
           !this->notIntegerIds.empty() ||
           !this->elementsAppearMoreThanOnce.empty();
}

void PuzzleParser::exportErrors(ofstream &outf) const {
    if (this->inValidNumberOfPieces) {
        outf << "Invalid number of elements" << endl;
        return;
    }
    this->printMissingElements(outf);
    this->printOutOfRangeElements(outf);
    this->printWrongFormatPieces(outf);
    this->printNotValidIdsElements(outf);
    this->printElementsAppearMoreThanOnce(outf);
}

void PuzzleParser::printMissingElements(ofstream &outf) const {
    if (this->missingPuzzleElements.empty())
        return;
    outf << "Missing puzzle element(s) with the following IDs:";
    bool firstElement = true;
    for (auto missingElement : this->missingPuzzleElements) {
        if (firstElement) {
            outf << " " << missingElement;
            firstElement = false;
        } else {
            outf << ", " << missingElement;
        }
    }
    outf << endl;
}

void PuzzleParser::printOutOfRangeElements(ofstream &outf) const {
    if (this->wrongPiecesIds.empty())
        return;

    outf << "Puzzle of size " << this->numberOfPieces << " cannot have the following IDs:";
    bool firstElement = true;

    for (auto outOfRangeElement : this->wrongPiecesIds) {
        if (firstElement) {
            outf << " " << outOfRangeElement;
            firstElement = false;
        } else
            outf << ", " << outOfRangeElement;
    }
    outf << endl;
}

void PuzzleParser::printWrongFormatPieces(ofstream &outf) const {
    if (this->wrongPieceFormatLine.empty())
        return;

    for (auto tuple : this->wrongPieceFormatLine) {
        outf << "Puzzle ID " << get<0>(tuple);
        outf << " has wrong data: " << get<1>(tuple) << endl;
    }
}

void PuzzleParser::printNotValidIdsElements(ofstream &outf) const {
    if (this->notIntegerIds.empty())
        return;

    outf << "The following element(s) doesn't has a valid id:";
    bool firstElement = true;

    for (auto &id : this->notIntegerIds) {
        if (firstElement) {
            outf << " " << id;
            firstElement = false;
        } else
            outf << ", " << id;
    }
    outf << endl;
}

void PuzzleParser::printElementsAppearMoreThanOnce(ofstream &outf) const {
    if (this->elementsAppearMoreThanOnce.empty())
        return;
    outf << "The following ids appear more than once:";
    bool firstElement = true;
    for (auto element : this->elementsAppearMoreThanOnce) {
        if (firstElement) {
            outf << " " << element;
            firstElement = false;
        } else
            outf << ", " << element;
    }
    outf << endl;
}