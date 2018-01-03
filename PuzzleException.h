#ifndef PUZZLE_PUZZLEEXCEPTION_H
#define PUZZLE_PUZZLEEXCEPTION_H

#include <iostream>
#include <fstream>
#include <memory>
#include <regex>

using namespace std;

#define NO_PROPER_SOLUTION "Cannot solve puzzle: it seems that there is no proper solution\n"
#define INVALID_NUMBER_OF_ELEMENTS "Invalid number of elements\n"
class PuzzleException {
public:
    explicit PuzzleException(const char* error);
    string errorMessage;
};


#endif //PUZZLE_PUZZLEEXCEPTION_H
