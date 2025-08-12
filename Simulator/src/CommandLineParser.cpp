#include "CommandLineParser.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include "Logger.h"
namespace fs = std::filesystem;
CommandLineParser CommandLineParser::parserInstance;

void CommandLineParser::scanForMode(int argc, char* argv[]) {
    LOG(LogLevel::INFO, "Scanning arguments for mode");
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == argumentStrings.comparativeMode){
            if(parsedArguments.mode != RunMode::Invalid)
                throw std::invalid_argument(errorMessages.multipleModesError);
            LOG(LogLevel::INFO, "Simulator mode set to comparative");
            parsedArguments.mode = RunMode::Comparative;
        }
        if (arg == argumentStrings.competitiveMode){
            if(parsedArguments.mode != RunMode::Invalid)
                throw std::invalid_argument(errorMessages.multipleModesError);
            LOG(LogLevel::INFO, "Simulator mode set to competition");
            parsedArguments.mode = RunMode::Competition;
        }
    }
    if(parsedArguments.mode == RunMode::Invalid){
        throw std::invalid_argument(errorMessages.missingMode);
    }
}

void CommandLineParser::parseComparativeMode(int argc, char *argv[]) {
    LOG(LogLevel::INFO, "Parsing comparative mode arguments");
    assert(parsedArguments.mode == RunMode::Comparative);
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if(arg == argumentStrings.verbose){
            parsedArguments.verbose = true;
            LOG(LogLevel::INFO, "Found optional argument -verbose");
            continue;
        }
        if(arg == argumentStrings.comparativeMode)
            continue;
        auto splitOpt = splitKeyValue(arg);
        if(!splitOpt.has_value()){
            throw std::invalid_argument(errorMessages.unexpectedArgument + arg + "\n");
        }
        auto [argKey, value] = splitOpt.value();
        const std::string dupError = errorMessages.duplicateArgument + argKey + "\n";
        if (argKey == argumentStrings.gameMap){
            if(parsedArguments.mapFileName.has_value())
                throw std::invalid_argument(dupError);
            parsedArguments.mapFileName = value;
        }else if (argKey == argumentStrings.managersFolder){
            if(parsedArguments.managersFolder.has_value())
                throw std::invalid_argument(dupError);
            parsedArguments.managersFolder = value;
        }else if (argKey == argumentStrings.algo1){
            if(parsedArguments.algo1.has_value())
                throw std::invalid_argument(dupError);
            parsedArguments.algo1 = value;
        }else if (argKey == argumentStrings.algo2){
            if(parsedArguments.algo2.has_value())
                throw std::invalid_argument(dupError);
            parsedArguments.algo2 = value;
        }else if (argKey == argumentStrings.numThreads){
            size_t pos = 0;
            try {
                parsedArguments.numThreads = std::stoi(value, &pos);
                if (pos != value.length() || parsedArguments.numThreads < 0) {
                    throw std::invalid_argument(errorMessages.invalidNumThreads);
                }
            }
            catch (const std::invalid_argument& e) {
                throw std::invalid_argument(errorMessages.invalidNumThreads);
            } catch (const std::out_of_range& e) {
                throw std::invalid_argument(errorMessages.invalidNumThreads);
            }
        }else{
            throw std::invalid_argument(errorMessages.unexpectedArgument + argKey + "\n");
        }
    }
}


void CommandLineParser::parseCompetitiveMode(int argc, char *argv[]) {
    LOG(LogLevel::INFO, "Parsing competition mode arguments");
    assert(parsedArguments.mode == RunMode::Competition);
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if(arg == argumentStrings.verbose){
            parsedArguments.verbose = true;
            continue;
        }
        if(arg == argumentStrings.competitiveMode)
            continue;
        auto splitOpt = splitKeyValue(arg);
        if(!splitOpt.has_value()){
            throw std::invalid_argument(errorMessages.unexpectedArgument + arg + "\n");
        }
        auto [argKey, value] = splitOpt.value();
        const std::string dupError = errorMessages.duplicateArgument + argKey + "\n";
        if (argKey == argumentStrings.mapsFolder){
            if(parsedArguments.mapsFolder.has_value())
                throw std::invalid_argument(dupError);
            parsedArguments.mapsFolder = value;
        }else if (argKey == argumentStrings.managerFile){
            if(parsedArguments.managerFileName.has_value())
                throw std::invalid_argument(dupError);
            parsedArguments.managerFileName = value;
        }else if (argKey == argumentStrings.algoFolder){
            if(parsedArguments.algoFolder.has_value())
                throw std::invalid_argument(dupError);
            parsedArguments.algoFolder = value;
        }else if (argKey == argumentStrings.numThreads){
            size_t pos = 0;
            try {
                parsedArguments.numThreads = std::stoi(value, &pos);
                if (pos != value.length() || parsedArguments.numThreads < 0) {
                    throw std::invalid_argument(errorMessages.invalidNumThreads);
                }
            }
            catch (const std::invalid_argument& e) {
                throw std::invalid_argument(errorMessages.invalidNumThreads);
            } catch (const std::out_of_range& e) {
                throw std::invalid_argument(errorMessages.invalidNumThreads);
            }
        }else{
            throw std::invalid_argument(errorMessages.unexpectedArgument + argKey + "\n");
        }
    }
}

void CommandLineParser::validateComparativeArguments() const {
    LOG(LogLevel::INFO, "Validating comparative mode arguments");
    std::string missingArguments = {};
    if (!parsedArguments.mapFileName.has_value())
        missingArguments += "game_map=<game_map_filename>\n";
    if (!parsedArguments.managersFolder.has_value())
        missingArguments += "game_managers_folder=<game_managers_folder>\n";;
    if (!parsedArguments.algo1.has_value())
        missingArguments += "algorithm1=<algorithm_so_filename>\n";
    if (!parsedArguments.algo2.has_value())
        missingArguments += "algorithm2=<algorithm_so_filename>\n";
    if (!missingArguments.empty()){
        throw std::invalid_argument(errorMessages.missingArguments + missingArguments);
    }
    if (!validSOFolder(parsedArguments.managersFolder.value())){
        throw std::invalid_argument(errorMessages.badDirectory + parsedArguments.managersFolder.value() + "\n");
    }
    if (!fileExists(parsedArguments.algo1.value())){
        throw std::invalid_argument(errorMessages.missingFile + parsedArguments.algo1.value() + "\n");
    }
    if (!fileExists(parsedArguments.algo2.value())){
        throw std::invalid_argument(errorMessages.missingFile + parsedArguments.algo2.value() + "\n");
    }
    if (!fileExists(parsedArguments.mapFileName.value())){
        throw std::invalid_argument(errorMessages.missingFile + parsedArguments.mapFileName.value() + "\n");
    }
    LOG(LogLevel::INFO, "All comparative mode arguments validated");
}

void CommandLineParser::validateCompetitiveArguments() const {
    LOG(LogLevel::INFO, "Validating competition mode arguments");
    std::string missingArguments = {};
    if (!parsedArguments.mapsFolder.has_value())
        missingArguments += "game_maps_folder=<game_maps_folder>\n";
    if (!parsedArguments.managerFileName.has_value())
        missingArguments += "game_manager=<game_manager_so_filename>\n";
    if (!parsedArguments.algoFolder.has_value())
        missingArguments += "algorithms_folder=<algorithms_folder>\n";
    if (!missingArguments.empty()){
        throw std::invalid_argument(errorMessages.missingArguments + missingArguments);
    }
    if (!validMapsFolder(parsedArguments.mapsFolder.value())){
        throw std::invalid_argument(errorMessages.badDirectory + parsedArguments.mapsFolder.value() + "\n");
    }
    if (!validSOFolder(parsedArguments.algoFolder.value())){
        throw std::invalid_argument(errorMessages.badDirectory + parsedArguments.algoFolder.value() + "\n");
    }
    if (!fileExists(parsedArguments.managerFileName.value())){
        throw std::invalid_argument(errorMessages.missingFile + parsedArguments.managerFileName.value() + "\n");
    }
    LOG(LogLevel::INFO, "All competition mode arguments validated");
}

void CommandLineParser::parse(int argc, char* argv[]) {
    LOG(LogLevel::INFO, "Starting to parse arguments");
    scanForMode(argc, argv);
    if (parsedArguments.mode == RunMode::Comparative){
        parseComparativeMode(argc, argv);
        validateComparativeArguments();
    }
    else if (parsedArguments.mode == RunMode::Competition){
        parseCompetitiveMode(argc, argv);
        validateCompetitiveArguments();
    }
    argumentsParsed = true;
}



std::optional<std::pair<std::string, std::string>> CommandLineParser::splitKeyValue(const std::string& arg) {
    auto pos = arg.find('=');
    if(pos != std::string::npos){
        return std::make_optional(std::make_pair(arg.substr(0, pos), arg.substr(pos + 1)));
    }
    return std::nullopt;
}


bool CommandLineParser::fileExists(const std::string& path) {
    LOG(LogLevel::INFO, std::string("Checking for existence of file: ") + path);
    std::ifstream f(path);
    return f.good();
}

bool CommandLineParser::validSOFolder(const std::string& path) {
    LOG(LogLevel::INFO, std::string("Validating so folder: ") + path);
    try {
        if (!fs::is_directory(path)) return false;

        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                const auto& filename = entry.path().filename().string();
                if (filename.size() > 3 && filename.substr(filename.size() - 3) == ".so") {
                    LOG(LogLevel::INFO, std::string("Found at least one so file in: ") + path);
                    return true;
                }
            }
        }
    } catch (...) {
        LOG(LogLevel::ERROR, std::string("Encountered error while validating: ") + path);
        return false;
    }
    LOG(LogLevel::ERROR, std::string("No so file found in: ") + path);
    return false;
}

bool CommandLineParser::validMapsFolder(const std::string &path) {
    try {
        if (!fs::is_directory(path)) return false;

        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                return true;
            }
        }
    } catch (...) {
        return false;
    }
    return false;

}