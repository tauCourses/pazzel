//
// Created by private on 1/3/18.
//

#include "PuzzleException.h"

PuzzleException::PuzzleException(const char *error) {
    this->errorMessage = string(error);
}
