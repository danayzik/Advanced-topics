#include "CompetitionSimulator.h"
#include "GameManagerRegistrar.h"
#include "AlgorithmRegistrar.h"
#include <filesystem>
#include "MapLoader.h"
#include <iostream>
#include <thread>
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
            std::string fileName = entry.path().stem().string();
            std::string fullPathStr = entry.path().string();
            try{
                maps.push_back(std::move(MapLoader::getInstance().loadMap(fullPathStr, errorBuffer)));
                maps.end()->mapFileName = fileName;
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
    scores.resize(algorithmRegistrar.count());
}

void CompetitionSimulator::loadArguments(const ParsedArguments &arguments) {
    Simulator::loadArguments(arguments);
    mapsFolder = arguments.mapsFolder.value();
    algorithmsFolder = arguments.algoFolder.value();
    loadGameManager(arguments.managerFileName.value());
    loadAlgorithms();
    loadMaps();
}

std::pair<size_t, size_t> CompetitionSimulator::fetchIndicesToRun() {
    const size_t n = scores.size();
    std::lock_guard<std::mutex>lock(indexFetchingMutex);
    size_t mapToRun = mapIndexToRun;
    size_t algoToRun = firstAlgoIndexToRun++;
    if (firstAlgoIndexToRun >= n){
        firstAlgoIndexToRun = 0;
        mapIndexToRun++;
    }
    return {mapToRun, algoToRun};
}

void CompetitionSimulator::storeGameResult(const GameResult &gameResult, size_t algo1Index, size_t algo2Index) {
    std::lock_guard<std::mutex>lock(storingResultsMutex);
    int winner = gameResult.winner;
    const auto& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();

    if (winner < 0 || winner > 2){
        errorBuffer << "Invalid game result between algorithm indexed " << algoRegistrar[algo1Index].name() << " and " << algoRegistrar[algo2Index].name() << "\n";
        return;
    }
    if (winner == 0){
        scores[algo1Index]++;
        scores[algo2Index]++;
    }
    else{
        scores[algo1Index] += 3 * (winner == 1);
        scores[algo2Index] += 3 * (winner == 2);
    }
}

void CompetitionSimulator::run() {
    const GameManagerRegistrar& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    const AlgorithmRegistrar& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    const auto& gmEntry = gameManagerRegistrar[0];
    const size_t n = scores.size();
    const size_t k = maps.size();
    auto runWorker = [&]() {
        while (true) {
            auto [mapToRunIndex, algoToRun] = fetchIndicesToRun();
            if (mapToRunIndex >= k) break;
            const auto &algoEntryOne = algorithmRegistrar[algoToRun];
            size_t secondAlgoToRunIndex = (algoToRun+1+(mapToRunIndex%(n-1))) % n;
            const auto &algoEntryTwo = algorithmRegistrar[secondAlgoToRunIndex];
            const auto& mapToRun = maps[mapToRunIndex];
            auto player1 = algoEntryOne.createPlayer(1, mapToRun.cols, mapToRun.rows, mapToRun.maxSteps, mapToRun.numShells);
            auto player2 = algoEntryOne.createPlayer(2, mapToRun.cols, mapToRun.rows, mapToRun.maxSteps, mapToRun.numShells);
            auto gm = gmEntry.createGameManager(verbose);
            try {
                const GameResult result = gm.get()->run(mapToRun.cols, mapToRun.rows,
                                                  *mapToRun.view.get(), mapToRun.mapFileName,
                                                  mapToRun.maxSteps, mapToRun.numShells,
                                                  *player1.get(), algoEntryOne.name(),
                                                  *player2.get(), algoEntryTwo.name(),
                                                  algoEntryOne.getTankAlgorithmFactory(), algoEntryTwo.getTankAlgorithmFactory());
                storeGameResult(std::move(result), algoToRun, secondAlgoToRunIndex);
            }
            catch (...) {
                std::lock_guard<std::mutex> lock(errorMutex);
                errorBuffer << "Game crashed between "
                            << algorithmRegistrar[algoToRun].name() << " and "
                            << algorithmRegistrar[secondAlgoToRunIndex].name() << " on map: "
                            << mapToRun.mapFileName << "\n";
            }

        }
    };
    std::vector<std::thread> threads;
    for (int i = 0; i < threadCount; ++i)
        threads.emplace_back(runWorker);

    runWorker();
    for (auto& t : threads)
        t.join();
    groupResults();
    printOutput();
}