#include "ComparativeSimulator.h"
#include "AlgorithmRegistrar.h"
#include <filesystem>
#include "GameManagerRegistrar.h"
namespace fs = std::filesystem;

void ComparativeSimulator::loadArguments(const ParsedArguments &arguments) {
    const std::string& algo1 = arguments.algo1.value();
    const std::string& algo2 = arguments.algo2.value();
    const std::string algo1Name = fs::path(algo1).filename().string();
    const std::string algo2Name = fs::path(algo2).filename().string();
    const std::string& mapPath = arguments.mapFileName.value();

    AlgorithmRegistrar& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    GameManagerRegistrar& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();

    algorithmRegistrar.createAlgorithmFactoryEntry(algo1Name);
    loadSO(algo1);
    if(algo1 == algo2){
        algorithmRegistrar.duplicateFirstEntry();
    }
    else{
        algorithmRegistrar.createAlgorithmFactoryEntry(algo2Name);
        loadSO(algo2);
    }


    mapFileName = fs::path(mapPath).filename().string();

    verbose = arguments.verbose;
    threads = arguments.numThreads;

    gameManagersFolder = arguments.managersFolder.value();

    for (const auto& entry : fs::directory_iterator(gameManagersFolder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            std::string fileName = entry.path().filename().string();
            std::string fullPathStr = entry.path().string();
            gameManagerRegistrar.createGameManagerFactoryEntry(fileName);
            loadSO(fullPathStr);
        }
    }

    gameManagerCount = gameManagerRegistrar.count();

}



// mapFile.open(mapPath);
//    if (!mapFile.is_open()) {
//        throw std::runtime_error("Failed to open file.");
//    }