#pragma once
#include "CommandLineParser.h"

class Simulator{
private:
    std::vector<void*> handles;

protected:
    void loadSO(const std::string& path);
    std::stringstream errorBuffer;

public:
    virtual void loadArguments(const ParsedArguments& arguments) = 0;
    virtual void run() = 0;
    virtual ~Simulator();

};