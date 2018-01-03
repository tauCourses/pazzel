#include "PuzzleParser.h"

PuzzleParser::PuzzleParser(ifstream &fin) : fin(fin) {}

void PuzzleParser::injectPieces(const unique_ptr<AbstractPieceManager> &pieceManager) {
    string line;
    int id;
    if (!this->tryReadFirstLine())
        throw PuzzleException(INVALID_NUMBER_OF_ELEMENTS);

    while (!this->fin.eof()) {
        getline(this->fin, line);
        if (!this->isStringEmpty(line))
            if ((id = this->getPieceId(line)) >= 0) //check if it's a valid id, return -1 if not
                pieceManager->addPiece(getNextPiece(id, line));
    }
    if (this->hasErrors(pieceManager))
        this->throwException();
}

bool PuzzleParser::tryReadFirstLine() {
    string line;
    getline(fin, line);
    auto number_token = line.substr(line.find_first_of('=') + 1);
    if (!this->isInteger(number_token))
        return false;

    this->numberOfPieces = std::stoi(number_token);
    if (this->numberOfPieces < 0)
        return false;

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

bool PuzzleParser::hasErrors(const unique_ptr<AbstractPieceManager> &pieceManager) {
    checkForMissingParts(pieceManager);
    return !this->missingPuzzleElements.empty() ||
           !this->wrongPiecesIds.empty() ||
           !this->wrongPieceFormatLine.empty() ||
           !this->notIntegerIds.empty() ||
           !this->elementsAppearMoreThanOnce.empty();
}

void PuzzleParser::throwException() const {
    stringstream exceptionMessage;
    this->printMissingElements(exceptionMessage);
    this->printOutOfRangeElements(exceptionMessage);
    this->printWrongFormatPieces(exceptionMessage);
    this->printNotValidIdsElements(exceptionMessage);
    this->printElementsAppearMoreThanOnce(exceptionMessage);
    throw PuzzleException(exceptionMessage.str());
}

void PuzzleParser::printMissingElements(stringstream &exceptionMessage) const {
    if (this->missingPuzzleElements.empty()) return;

    exceptionMessage << MISSING_PUZZLE_ELEMENTS;
    for (unsigned int i = 0; i < this->missingPuzzleElements.size(); ++i)
        exceptionMessage << (i == 0 ? "" : ", ") << this->missingPuzzleElements[i];
    exceptionMessage << endl;
}

void PuzzleParser::printOutOfRangeElements(stringstream &exceptionMessage) const {
    if (this->wrongPiecesIds.empty()) return;

    exceptionMessage << OUT_OF_RANGE_PIECE_INDEX_PART1 << this->numberOfPieces << OUT_OF_RANGE_PIECE_INDEX_PART2;
    for (unsigned int i = 0; i < this->wrongPiecesIds.size(); ++i)
        exceptionMessage << (i == 0 ? "" : ", ") << this->wrongPiecesIds[i];
    exceptionMessage << endl;
}

void PuzzleParser::printWrongFormatPieces(stringstream &exceptionMessage) const {
    if (this->wrongPieceFormatLine.empty()) return;

    for (auto tuple : this->wrongPieceFormatLine)
        exceptionMessage << WRONG_PIECE_FORMAT_PART1 << get<0>(tuple)
                         << WRONG_PIECE_FORMAT_PART2 << get<1>(tuple) << endl;
}

void PuzzleParser::printNotValidIdsElements(stringstream &exceptionMessage) const {
    if (this->notIntegerIds.empty()) return;

    exceptionMessage << INVALID_ID_PUZZLE_ELEMENT;
    for (unsigned int i = 0; i < this->notIntegerIds.size(); ++i)
        exceptionMessage << (i == 0 ? "" : ", ") << this->notIntegerIds[i];
    exceptionMessage << endl;
}


void PuzzleParser::printElementsAppearMoreThanOnce(stringstream &exceptionMessage) const {
    if (this->elementsAppearMoreThanOnce.empty()) return;

    exceptionMessage << DUPLICATED_PUZZLE_ELEMENT_ID;
    for (unsigned int i = 0; i < this->elementsAppearMoreThanOnce.size(); ++i)
        exceptionMessage << (i == 0 ? "" : ", ") << this->elementsAppearMoreThanOnce[i];
    exceptionMessage << endl;
}