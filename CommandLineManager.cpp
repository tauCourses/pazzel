#include "CommandLineManager.h"

CommandLineManager::CommandLineManager(int argc, char** argv) : numberOfArguments(argc)
{
    if(argc<3 || argc >4) {
        wrongNumberOfArguments = true;
        return;
    }
    if(argc==4)
        if(!updateRotateParamInx(argc, argv))
            return;
    updateIOStreams(argv);
}

bool CommandLineManager::isRotateEnable() const {
    return this->_rotateEnabled;
}

bool CommandLineManager::hasErrors() const {
    return this->wrongNumberOfArguments || this->rotateParamDoesntExist || this->unableToReadInputFile;
}

void CommandLineManager::exportErrors() const {
    if(this->wrongNumberOfArguments)
        std::cerr << "Wrong number of command line arguments" << "\n";
    if(this->rotateParamDoesntExist)
        std::cerr << "No '-rotate' argument found" << "\n";
    if(this->unableToReadInputFile)
        std::cerr << "Unable to open input file" << "\n";
}

bool CommandLineManager::updateRotateParamInx(int argc, char **argv) {
    string rotateStr ("-rotate");
    for(int i=1; i<argc; i++)
    {
        if(rotateStr == argv[i])
        {
            this->rotateParamInx = i;
            return true;
        }
    }
    this->rotateParamDoesntExist = true;
    return false;
}

void CommandLineManager::updateIOStreams(char **argv) {
    int currentInx = 0;
    currentInx = this->getNextIndex(currentInx);
    this->inputStream = ifstream(argv[currentInx], ifstream::in);
    if(!this->inputStream.good()) {
        this->unableToReadInputFile = true;
        return;
    }
    currentInx = this->getNextIndex(currentInx);
    this->outputStream = ofstream(argv[currentInx], ifstream::out);
}

int CommandLineManager::getNextIndex(int current) {
    if(current+1 == this->rotateParamInx)
        return current+2;
    return current+1;
}


