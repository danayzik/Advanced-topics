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
                throw std::invalid_argument(errorMessages.multipleModesError);
            parsedArguments.mode = RunMode::Comparative;
        }
        if (arg == argumentStrings.competitiveMode){
            if(parsedArguments.mode != RunMode::Invalid)
                throw std::invalid_argument(errorMessages.multipleModesError);
            parsedArguments.mode = RunMode::Competition;
        }
    }
    if(parsedArguments.mode == RunMode::Invalid){
        throw std::invalid_argument(errorMessages.missingMode);
    }
}

void CommandLineParser::parseComparativeMode(int argc, char *argv[]) {
    assert(parsedArguments.mode == RunMode::Comparative);
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if(arg == argumentStrings.verbose){
            parsedArguments.verbose = true;
            continue;
        }
        if(arg == argumentStrings.comparativeMode)
            continue;
        auto splitOpt = splitKeyValue(arg);
        if(!splitOpt.has_value()){
            throw std::invalid_argument(errorMessages.unexpectedArgument);
        }
        auto [argKey, value] = splitOpt.value();
        if (argKey == argumentStrings.gameMap){
            if(parsedArguments.mapFileName.has_value())
                throw std::invalid_argument(errorMessages.duplicateArgument);
            parsedArguments.mapFileName = value;
        }else if (argKey == argumentStrings.managersFolder){
            if(parsedArguments.managersFolder.has_value())
                throw std::invalid_argument(errorMessages.duplicateArgument);
            parsedArguments.managersFolder = value;
        }else if (argKey == argumentStrings.algo1){
            if(parsedArguments.algo1.has_value())
                throw std::invalid_argument(errorMessages.duplicateArgument);
            parsedArguments.algo1 = value;
        }else if (argKey == argumentStrings.algo2){
            if(parsedArguments.algo2.has_value())
                throw std::invalid_argument(errorMessages.duplicateArgument);
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
            throw std::invalid_argument(errorMessages.unexpectedArgument);
        }
    }
}


void CommandLineParser::parseCompetitiveMode(int argc, char *argv[]) {
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
            throw std::invalid_argument(errorMessages.unexpectedArgument);
        }
        auto [argKey, value] = splitOpt.value();
        if (argKey == argumentStrings.mapsFolder){
            if(parsedArguments.mapsFolder.has_value())
                throw std::invalid_argument(errorMessages.duplicateArgument);
            parsedArguments.mapsFolder = value;
        }else if (argKey == argumentStrings.managerFile){
            if(parsedArguments.managersFileName.has_value())
                throw std::invalid_argument(errorMessages.duplicateArgument);
            parsedArguments.managersFileName = value;
        }else if (argKey == argumentStrings.algoFolder){
            if(parsedArguments.algoFolder.has_value())
                throw std::invalid_argument(errorMessages.duplicateArgument);
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
            throw std::invalid_argument(errorMessages.unexpectedArgument);
        }
    }
}

void CommandLineParser::validateComparativeArguments() const {
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
        throw std::invalid_argument(errorMessages.badSODDirectory + parsedArguments.managersFolder.value() + "\n");
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
}

void CommandLineParser::validateCompetitiveArguments() const {
    std::string missingArguments = {};
    if (!parsedArguments.mapsFolder.has_value())
        missingArguments += "game_maps_folder=<game_maps_folder>\n";
    if (!parsedArguments.managersFileName.has_value())
        missingArguments += "game_manager=<game_manager_so_filename>\n";
    if (!parsedArguments.algoFolder.has_value())
        missingArguments += "algorithms_folder=<algorithms_folder>\n";
    if (!missingArguments.empty()){
        throw std::invalid_argument(errorMessages.missingArguments + missingArguments);
    }
    if (!validSOFolder(parsedArguments.mapsFolder.value())){
        throw std::invalid_argument(errorMessages.badSODDirectory + parsedArguments.mapsFolder.value() + "\n");
    }
    if (!validSOFolder(parsedArguments.algoFolder.value())){
        throw std::invalid_argument(errorMessages.badSODDirectory + parsedArguments.algoFolder.value() + "\n");
    }
    if (!fileExists(parsedArguments.managersFileName.value())){
        throw std::invalid_argument(errorMessages.missingFile + parsedArguments.managersFileName.value() + "\n");
    }
}

void CommandLineParser::parse(int argc, char* argv[]) {
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
    std::ifstream f(path);
    return f.good();
}

bool CommandLineParser::validSOFolder(const std::string& path) {
    try {
        if (!fs::is_directory(path)) return false;

        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                const auto& filename = entry.path().filename().string();
                if (filename.size() > 3 && filename.substr(filename.size() - 3) == ".so") {
                    return true;
                }
            }
        }
    } catch (...) {
        return false;
    }
    return false;
}