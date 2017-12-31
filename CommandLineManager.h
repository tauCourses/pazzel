#ifndef PUZZLE_COMMANDLINEMANAGER_H
#define PUZZLE_COMMANDLINEMANAGER_H

#include <iostream>
#include <fstream>
#include <memory>
#include <regex>

using namespace std;

class CommandLineManager {
private:
    int numberOfArguments;
    int numberOfThreads = 4;
    bool wrongNumberOfArguments = false;
    bool notValidNumberOfThreads = false;
    bool unableToOpenInputFile = false;
    bool _rotateEnabled = false;

    bool tryOpenInputStream(char *fileName);
public:
    ifstream inputStream;
    ofstream outputStream;

    explicit CommandLineManager(int argc, char **argv);

    ~CommandLineManager();

    bool hasErrors() const;

    void exportErrors() const;

    bool isRotateEnable() const;

    bool checkForValidNumberOfThreads(int argc, char **argv, int index);

    int getNumberOfThreads();
};

#endif //PUZZLE_COMMANDLINEMANAGER_H
