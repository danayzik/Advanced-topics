#pragma once

#include <string>
#include <map>
#include <vector>
#include <optional>

enum class RunMode {
    Comparative,
    Competition,
    Invalid
};


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
        const std::string missingMode = "Error: missing mandatory mode argument\n";
    };

    struct ParsedArguments {
        RunMode mode = RunMode::Invalid;
        bool verbose = false;
        std::optional<std::string> mapFileName;
        std::optional<std::string> managersFolder;
        std::optional<std::string> algo1;
        std::optional<std::string> algo2;
        std::optional<std::string> mapsFolder;
        std::optional<std::string> managersFileName;
        std::optional<std::string> algoFolder;
        int num_threads = 1;
    };

    ArgumentStrings argumentStrings = {};
    ErrorMessages errorMessages = {};
    static CommandLineParser parserInstance;
    ParsedArguments parsedArguments;
    void scanForMode(int argc, char* argv[]);
    void parseCompetitiveMode(int argc, char* argv[]);
    void parseComparativeMode(int argc, char* argv[]);
    std::optional<std::pair<std::string, std::string>>  splitKeyValue(const std::string& arg);

    void validateRequiredKeys(const ParsedArguments& args, std::vector<std::string>& errors);
    void validatePaths(const ParsedArguments& args, std::vector<std::string>& errors);
    bool fileExists(const std::string& path);
    bool validFolder(const std::string& path);


public:
    static inline CommandLineParser& getParser(){return parserInstance;}
    ParsedArguments& parse(int argc, char* argv[]);
    void printUsageAndExit(const std::vector<std::string>& errors);
};
