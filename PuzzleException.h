#ifndef PUZZLE_PUZZLEEXCEPTION_H
#define PUZZLE_PUZZLEEXCEPTION_H

#include <iostream>
#include <fstream>
#include <memory>
#include <regex>

using namespace std;

#define NO_PROPER_SOLUTION "Cannot solve puzzle: it seems that there is no proper solution\n"
#define SUM_OF_EDGES_IS_NOT_ZERO "Cannot solve puzzle: sum of edges is not zero"
#define WRONG_NUMBER_OF_STRAIGHT_EDGES "Cannot solve puzzle: wrong number of straight edges"
#define INVALID_NUMBER_OF_ELEMENTS "Invalid number of elements\n"
#define MISSING_PUZZLE_ELEMENTS "Missing puzzle element(s) with the following IDs: "
#define OUT_OF_RANGE_PIECE_INDEX_PART1 "Puzzle of size "
#define OUT_OF_RANGE_PIECE_INDEX_PART2 " cannot have the following IDs: "
#define WRONG_PIECE_FORMAT_PART1 "Puzzle ID "
#define WRONG_PIECE_FORMAT_PART2 " has wrong data: "
#define INVALID_ID_PUZZLE_ELEMENT "The following element(s) doesn't has a valid id: "
#define DUPLICATED_PUZZLE_ELEMENT_ID "The following ids appear more than once: "


class PuzzleException {
public:
    explicit PuzzleException(string error);

    string errorMessage;
};


#endif //PUZZLE_PUZZLEEXCEPTION_H
