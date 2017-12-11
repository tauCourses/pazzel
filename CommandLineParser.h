#ifndef PUZZLE_COMMANDLINEPARSER_H
#define PUZZLE_COMMANDLINEPARSER_H


class CommandLineParser {
private:
    int rotateArgumentLocation(int argc, char** argv); //return -1 if no rotate argument
    bool inputFileReadable(char* input);
public:
    explicit CommandLineParser(int argc, char** argv);
};

#endif //PUZZLE_COMMANDLINEPARSER_H
