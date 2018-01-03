#include "CommandLineManager.h"

CommandLineManager::CommandLineManager(int argc, char **argv) : numberOfArguments(argc) {
    string rotateStr("-rotate");
    string threadsStr("-threads");
    bool inputStreamParamFound = false;
    bool outputStreamParamFound = false;
    for(int i=1; i<argc; i++)
    {
        if (rotateStr == argv[i])
            this->_rotateEnabled = true;
        else if (threadsStr == argv[i])
        {
            if(!checkForValidNumberOfThreads(argc,argv,i+1))
                return;
            i++;
        }
        else if(!inputStreamParamFound) {
            if (!tryOpenInputStream(argv[i]))
                return;
            inputStreamParamFound = true;
        }
        else if(!outputStreamParamFound) {
            this->outputStream.open(argv[i], ifstream::out);
            outputStreamParamFound = true;
        }
        else {
            this->wrongNumberOfArguments = true;
            return;
        }
    }
    if(!inputStreamParamFound || !outputStreamParamFound)
        this->wrongNumberOfArguments = true;
}

bool CommandLineManager::isRotateEnable() const {
    return this->_rotateEnabled;
}

bool CommandLineManager::hasErrors() const {
    if(this->wrongNumberOfArguments || this->unableToOpenInputFile || this->notValidNumberOfThreads)
    {
        this->exportErrors();
        return true;
    }
    return false;
}

void CommandLineManager::exportErrors() const {
    if (this->wrongNumberOfArguments)
        std::cerr << "Wrong number of command line arguments" << endl;
    if (this->notValidNumberOfThreads)
        std::cerr << "Number of threads is not valid" << endl;
    if (this->unableToOpenInputFile)
        std::cerr << "Unable to open input file" << endl;
}

bool CommandLineManager::tryOpenInputStream(char *fileName) {
    this->inputStream.open(fileName, ifstream::in);
    if (!this->inputStream.good()) {
        this->unableToOpenInputFile = true;
        return false;
    }
    return true;
}

CommandLineManager::~CommandLineManager() {
    if (inputStream.is_open())
        inputStream.close();
    if (outputStream.is_open())
        outputStream.close();
}

bool CommandLineManager::checkForValidNumberOfThreads(int argc, char **argv, int index) {
    if(index >= argc)
    {
        this->notValidNumberOfThreads = true;
        return false;
    }
    string numberOfThreads(argv[index]);
    static std::regex isNumber("^[1-9][0-9]*$");
    if(std::regex_match(numberOfThreads, isNumber))
    {
        this->numberOfThreads = std::stoi(numberOfThreads);
        return true;
    }
    this->notValidNumberOfThreads = true;
    return false;
}

int CommandLineManager::getNumberOfThreads() {
    return this->numberOfThreads;
}

