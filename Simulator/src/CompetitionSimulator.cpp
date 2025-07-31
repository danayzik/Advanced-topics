#include "CompetitionSimulator.h"
#include "GameManagerRegistrar.h"
#include "AlgorithmRegistrar.h"
#include <filesystem>
#include "MapLoader.h"
#include <iostream>

namespace fs = std::filesystem;

void CompetitionSimulator::loadGameManager(const std::string &gameManagerSoPath) {
    const std::string managerName = fs::path(gameManagerSoPath).stem().string();
    auto& managerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    managerRegistrar.createGameManagerFactoryEntry(managerName);
    loadSO(gameManagerSoPath);
    managerRegistrar.validateLastRegistration();
}

void CompetitionSimulator::loadMaps() {
    auto& mapLoader = MapLoader::getInstance();
    for (const auto& entry : fs::directory_iterator(mapsFolder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
//            std::string fileName = entry.path().stem().string();
            std::string fullPathStr = entry.path().string();
            try{
                maps.push_back(std::move(MapLoader::getInstance().loadMap(fullPathStr, errorBuffer)));
            }
            catch (const std::exception& e) {
                std::cout << "Failed loading map file: " << fullPathStr << "\n" << e.what();
                errorBuffer << "Failed loading map file: " << fullPathStr << "\n" << e.what();
            }
        }
    }
    if (maps.empty()){
        throw NotEnoughValidMaps("No valid maps loaded for competition\n");
    }
}

void CompetitionSimulator::loadAlgorithms() {
    AlgorithmRegistrar& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    for (const auto& entry : fs::directory_iterator(algorithmsFolder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            std::string fullPathStr = entry.path().string();
            try{
                loadSO(fullPathStr);
                algorithmRegistrar.validateLastRegistration();
            }
            catch (const SharedObjectLoadingException& e) {
                std::cout << "Failed loading algo so: " << fullPathStr << "\n" << e.what();
                errorBuffer << "Failed loading algo so: " << fullPathStr << "\n" << e.what();
            }
            catch (const BadRegistrationException& e) {
                algorithmRegistrar.removeLast();
                std::cout << "Failed registering so: " << fullPathStr << "\n" << e.what();
                errorBuffer << "Failed registering so: " << fullPathStr << "\n" << e.what();
            }
        }
    }
    if (algorithmRegistrar.count() < 2){
        throw NotEnoughValidAlgorithms("Less than 2 valid algorithms loaded for competition\n");
    }

}

void CompetitionSimulator::loadArguments(const ParsedArguments &arguments) {
    Simulator::loadArguments(arguments);
    mapsFolder = arguments.mapsFolder.value();
    algorithmsFolder = arguments.algoFolder.value();
    loadGameManager(arguments.managerFileName.value());
    loadAlgorithms();
    loadMaps();
}