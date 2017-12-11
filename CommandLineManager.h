#ifndef PUZZLE_COMMANDLINEMANAGER_H
#define PUZZLE_COMMANDLINEMANAGER_H

#include <iostream>
#include <fstream>

using namespace std;

class CommandLineManager {
private:
    int numberOfArguments;
    bool unableToReadInputFile;
    bool _rotateEnabled = false;

    int rotateArgumentLocation(int argc, char** argv); //return -1 if no rotate argument
    bool inputFileReadable(char* input);
    bool updateNumberOfArguments(); //return false when wrong number of arrguments
public:
    ifstream inputStream;
    ofstream outputStream;
    explicit CommandLineManager(int argc, char** argv);
    bool hasErrors() const;
    void exportErrors() const;
    bool isRotateEnable() const;

};

#endif //PUZZLE_COMMANDLINEMANAGER_H
