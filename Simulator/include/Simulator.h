#pragma once
#include "CommandLineParser.h"
#include "GameResult.h"
#include <sstream>

class Simulator{
private:
    std::vector<void*> handles;

protected:
    int threadCount = {};
    bool verbose = false;
    void loadSO(const std::string& path);
    std::stringstream errorBuffer;
    std::string getTimeString();

public:
    virtual void loadArguments(const ParsedArguments& arguments) = 0;
    virtual void run() = 0;
    virtual ~Simulator();

};