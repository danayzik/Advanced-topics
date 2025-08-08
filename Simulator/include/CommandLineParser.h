#pragma once

#include <string>
#include <map>
#include <vector>
#include <optional>
#include "Arguments.h"
#include <cassert>


class CommandLineParser {
private:
    struct ArgumentStrings{
        const std::string comparativeMode = "-comparative";
        const std::string competitiveMode = "-competition";
        const std::string gameMap = "game_map";
        const std::string managersFolder = "game_managers_folder";
        const std::string algo1 = "algorithm1";
        const std::string algo2 = "algorithm2";
        const std::string numThreads = "num_threads";
        const std::string verbose = "-verbose";
        const std::string mapsFolder = "game_maps_folder";
        const std::string managerFile = "game_manager";
        const std::string algoFolder = "algorithms_folder";
    };

    struct ErrorMessages{
        const std::string ProperUsageMessage =
            "Example of proper usage:\n"
            "  -comparative game_map=<game_map_filename> game_managers_folder=<game_managers_folder> algorithm1=<algorithm1_so> algorithm2=<algorithm2_so> [num_threads=<num>] [-verbose]\n"
            "  -competition game_maps_folder=<game_maps_folder> game_manager=<game_manager_so> algorithms_folder=<algorithms_folder> [num_threads=<num>] [-verbose]\n";
        const std::string multipleModesError = "Error: Both '-comparative' and '-competition' modes were specified.\nPlease choose exactly one mode.\n";
        const std::string duplicateArgument = "Error: duplicate argument detected\n";
        const std::string missingMode = "Error: missing mandatory mode argument\n";
        const std::string invalidNumThreads = "Error: invalid num_threads value\n";
        const std::string unexpectedArgument = "Error: unexpected argument\n";
        const std::string missingArguments = "Error, the following arguments are missing: ";
        const std::string badDirectory = "Error, the following folder path provided is invalid or contains no valid files: ";
        const std::string missingFile = "Error, the following file path is invalid: ";
    };


    bool argumentsParsed = false;
    ArgumentStrings argumentStrings = {};
    ErrorMessages errorMessages = {};
    static CommandLineParser parserInstance;
    ParsedArguments parsedArguments;
    void scanForMode(int argc, char* argv[]);
    void parseCompetitiveMode(int argc, char* argv[]);
    void parseComparativeMode(int argc, char* argv[]);
    void validateCompetitiveArguments() const;
    void validateComparativeArguments() const;

    static std::optional<std::pair<std::string, std::string>>  splitKeyValue(const std::string& arg);
    static bool fileExists(const std::string& path) ;
    static bool validSOFolder(const std::string& path);
    static bool validMapsFolder(const std::string& path);
    CommandLineParser() = default;

public:
    CommandLineParser(const CommandLineParser&) = delete;
    CommandLineParser& operator=(const CommandLineParser&) = delete;
    CommandLineParser(CommandLineParser&&) = delete;
    CommandLineParser& operator=(CommandLineParser&&) = delete;
    ~CommandLineParser() = default;

    static inline CommandLineParser& getParser(){return parserInstance;}
    void parse(int argc, char* argv[]);
    [[nodiscard]] inline const ParsedArguments& getParsedArguments() const{ assert(argumentsParsed); return parsedArguments;}

};
