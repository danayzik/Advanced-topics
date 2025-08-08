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
    static std::string getTimeString();
    static bool validGameResult(const GameResult& result);
    static void writeResultsToFile(const std::stringstream &resultStream, const std::string& filePrefix, const std::string& folder);

public:
    virtual void loadArguments(const ParsedArguments& arguments);
    virtual void run() = 0;
    virtual ~Simulator();

};