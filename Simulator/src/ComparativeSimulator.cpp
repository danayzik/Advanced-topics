#include "ComparativeSimulator.h"
#include "AlgorithmRegistrar.h"
#include <filesystem>
#include "GameManagerRegistrar.h"
namespace fs = std::filesystem;
// Get rid of .so extension in factory making
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

    mapInfo = std::move(MapLoader::getInstance().loadMap(mapPath, errorBuffer));

    verbose = arguments.verbose;
    threads = arguments.numThreads;

    gameManagersFolder = arguments.managersFolder.value();

    //Skip bad game managers, try and catch on loading manager so
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

void ComparativeSimulator::run() {
    GameManagerRegistrar& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    auto runWorker = [&]() {
        while (true) {
            size_t i = nextIndex.fetch_add(1);
            if (i >= gameManagerRegistrar.count()) break;

            const auto& entry = gameManagerRegistrar[i];
            auto gm = entry.createGameManager(verbose);
            gm.get()->run(mapInfo.cols, mapInfo.rows, mapInfo.view.get(), mapFileName, mapInfo.maxSteps, mapInfo.numShells, )
             // your existing function
            // store result somewhere thread-safe
        }
    };
}

