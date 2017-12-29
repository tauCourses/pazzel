#ifndef PUZZLE_COMMANDLINEMANAGER_H
#define PUZZLE_COMMANDLINEMANAGER_H

#include <iostream>
#include <fstream>
#include <memory>

using namespace std;

class CommandLineManager {
private:
    int numberOfArguments;
    int rotateParamInx;
    int numberOfThreads = 4;
    bool wrongNumberOfArguments = false;
    bool rotateParamDoesntExist = false;
    bool unableToReadInputFile = false;
    bool _rotateEnabled = false;

    bool updateRotateParamInx(int argc, char **argv); //return false if no rotate param found
    void updateIOStreams(char **argv); //return false if failed
    int getNextIndex(int current); //return the next available cmd argument
public:
    ifstream inputStream;
    ofstream outputStream;

    explicit CommandLineManager(int argc, char **argv);

    ~CommandLineManager();

    bool hasErrors() const;

    void exportErrors() const;

    bool isRotateEnable() const;

    int getNumberOfThreads() const;


};

#endif //PUZZLE_COMMANDLINEMANAGER_H
