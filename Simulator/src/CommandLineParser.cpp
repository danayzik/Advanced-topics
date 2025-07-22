#include "CommandLineParser.h"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;


void CommandLineParser::scanForMode(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == argumentStrings.comparativeMode){
            if(parsedArguments.mode != RunMode::Invalid)
                throw std::runtime_error(errorMessages.multipleModesError);
            parsedArguments.mode = RunMode::Comparative;
        }
        if (arg == argumentStrings.competitiveMode){
            if(parsedArguments.mode != RunMode::Invalid)
                throw std::runtime_error(errorMessages.multipleModesError);
            parsedArguments.mode = RunMode::Competition;
        }
    }
    if(parsedArguments.mode == RunMode::Invalid){
        throw std::runtime_error(errorMessages.missingMode);
    }
}

ParsedArguments& CommandLineParser::parse(int argc, char* argv[]) {
    ParsedArguments result;
    std::vector<std::string> unknownArgs;

    if (argc < 2) {
        printUsageAndExit({"Missing mode argument (-comparative or -competition)"});
    }

    std::string modeArg = argv[1];
    if (modeArg == "-comparative") result.mode = RunMode::Comparative;
    else if (modeArg == "-competition") result.mode = RunMode::Competition;
    else printUsageAndExit({"Invalid mode argument: " + modeArg});
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-verbose") {
            result.verbose = true;
        } else if (arg.find('=') != std::string::npos) {
            auto keyValue = splitKeyValue(arg);
            result.options[keyValue[0]] = keyValue[1];
        } else {
            unknownArgs.push_back(arg);
        }
    }

    std::vector<std::string> errors;
    if (!unknownArgs.empty()) {
        for (const auto& arg : unknownArgs) errors.push_back("Unknown argument: " + arg);
    }

    validateRequiredKeys(result, errors);
    validatePaths(result, errors);

    if (!errors.empty()) {
        printUsageAndExit(errors);
    }

    return result;
}

void CommandLineParser::printUsageAndExit(const std::vector<std::string>& errors) {
    std::cerr << "Usage:\n"
              << "./simulator_<submitter_ids> -comparative game_map=<game_map_filename> game_managers_folder=<game_managers_folder> algorithm1=<algorithm_so_filename> algorithm2=<algorithm_so_filename> [num_threads=<num>] [-verbose]\n"
              << "./simulator_<submitter_ids> -competition game_maps_folder=<game_maps_folder> game_manager=<game_manager_so_filename> algorithms_folder=<algorithms_folder> [num_threads=<num>] [-verbose]\n\n";
    for (const auto& error : errors) {
        std::cerr << "Error: " << error << std::endl;
    }
    exit(1);
}

std::vector<std::string> CommandLineParser::splitKeyValue(const std::string& arg) {
    auto pos = arg.find('=');
    return { arg.substr(0, pos), arg.substr(pos + 1) };
}

void CommandLineParser::validateRequiredKeys(const ParsedArguments& args, std::vector<std::string>& errors) {
    std::vector<std::string> requiredKeys;
    if (args.mode == RunMode::Comparative) {
        requiredKeys = {"game_map", "game_managers_folder", "algorithm1", "algorithm2"};
    } else if (args.mode == RunMode::Competition) {
        requiredKeys = {"game_maps_folder", "game_manager", "algorithms_folder"};
    }

    for (const auto& key : requiredKeys) {
        if (args.options.find(key) == args.options.end()) {
            errors.push_back("Missing required argument: " + key);
        }
    }
}

void CommandLineParser::validatePaths(const ParsedArguments& args, std::vector<std::string>& errors) {
    for (const auto& [key, value] : args.options) {
        if (key.find("file") != std::string::npos || key.find("map") != std::string::npos) {
            if (!fileExists(value) && !validFolder(value)) {
                errors.push_back("Invalid file/folder path: " + key + "=" + value);
            }
        }
    }
}

bool CommandLineParser::fileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

bool CommandLineParser::validFolder(const std::string& path) {
    try {
        if (!fs::is_directory(path)) return false;
        for (auto& p : fs::directory_iterator(path)) {
            if (p.is_regular_file()) return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}