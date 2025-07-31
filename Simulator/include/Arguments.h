#pragma once
#include <string>

enum class RunMode {
    Comparative,
    Competition,
    Invalid
};

struct ParsedArguments {
    RunMode mode = RunMode::Invalid;
    bool verbose = false;
    std::optional<std::string> mapFileName = {};
    std::optional<std::string> managersFolder = {};
    std::optional<std::string> algo1 = {};
    std::optional<std::string> algo2 = {};
    std::optional<std::string> mapsFolder = {};
    std::optional<std::string> managerFileName = {};
    std::optional<std::string> algoFolder = {};
    int numThreads = 1;
};