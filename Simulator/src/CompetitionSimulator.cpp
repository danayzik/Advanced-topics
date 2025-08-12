#include "CompetitionSimulator.h"
#include "GameManagerRegistrar.h"
#include "AlgorithmRegistrar.h"
#include <filesystem>
#include "MapLoader.h"
#include <iostream>
#include <thread>
#include <cassert>
#include <algorithm>
#include "Logger.h"
namespace fs = std::filesystem;

void CompetitionSimulator::loadGameManager(const std::string &gameManagerSoPath) {
    LOG(LogLevel::INFO, std::string("Attempting to load game manager from: ") + gameManagerSoPath);
    const std::string managerName = fs::path(gameManagerSoPath).stem().string();
    managerFileName = managerName;
    auto& managerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    managerRegistrar.createGameManagerFactoryEntry(managerName);
    loadSO(gameManagerSoPath);
    managerRegistrar.validateLastRegistration();
}

void CompetitionSimulator::loadMaps() {
    LOG(LogLevel::INFO, std::string("Attempting to load maps from: ") + mapsFolder);
    auto& mapLoader = MapLoader::getInstance();
    for (const auto& entry : fs::directory_iterator(mapsFolder)) {
        if (entry.is_regular_file()) {
            std::string fileName = entry.path().stem().string();
            std::string fullPathStr = entry.path().string();
            try{
                maps.push_back(mapLoader.loadMap(fullPathStr, errorBuffer));
                maps.back().mapFileName = fileName;
            }
            catch (const std::exception& e) {
                std::cout << "Failed loading map file: " << fullPathStr << "\n" << e.what() << "\n";
                errorBuffer << "Failed loading map file: " << fullPathStr << "\n" << e.what() << "\n";
            }
        }
    }
    if (maps.empty()){
        throw NotEnoughValidMaps("No valid maps loaded for competition\n");
    }
    LOG(LogLevel::INFO, std::string("Successfully loaded ") + std::to_string(maps.size()) + " maps");
}

void CompetitionSimulator::loadAlgorithms() {
    LOG(LogLevel::INFO, std::string("Attempting to load algorithms from ") + algorithmsFolder);
    AlgorithmRegistrar& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    for (const auto& entry : fs::directory_iterator(algorithmsFolder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            std::string fullPathStr = entry.path().string();
            std::string fileName = entry.path().stem().string();
            try{
                scores.emplace_back(fileName, 0);
                algorithmRegistrar.createAlgorithmFactoryEntry(fileName);
                loadSO(fullPathStr);
                algorithmRegistrar.validateLastRegistration();
            }
            catch (const SharedObjectLoadingException& e) {
                scores.pop_back();
                algorithmRegistrar.removeLast();
                std::cout <<  e.what() << "\n";
            }
            catch (const BadRegistrationException& e) {
                scores.pop_back();
                algorithmRegistrar.removeLast();
                std::cout << "Failed registering so: " << fullPathStr << "\n" << e.what();
                errorBuffer << "Failed registering so: " << fullPathStr << "\n" << e.what();
            }
        }
    }
    if (algorithmRegistrar.count() < 2){
        throw NotEnoughValidAlgorithms("Less than 2 valid algorithms loaded for competition\n");
    }
    LOG(LogLevel::INFO, std::string("Successfully loaded ") + std::to_string(algorithmRegistrar.count()) + " algorithms");
    assert(scores.size() == algorithmRegistrar.count());

}

void CompetitionSimulator::loadArguments(const ParsedArguments &arguments) {
    LOG(LogLevel::INFO, "Loading parsed arguments into the competition simulator");
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

    if (!validGameResult(gameResult)){
        errorBuffer << "Invalid game result between algorithms indexed "
        << algoRegistrar[algo1Index].name() << " and "
        << algoRegistrar[algo2Index].name() << ". Result not counted"
        << "\n";
        return;
    }
    if (winner == 0){
        scores[algo1Index].second++;
        scores[algo2Index].second++;
    }
    else{
        scores[algo1Index].second += 3 * (winner == 1);
        scores[algo2Index].second += 3 * (winner == 2);
    }
}


void CompetitionSimulator::sortScores() {
    LOG(LogLevel::INFO, "Sorting algorithms by scores");
    std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second)
            return a.second > b.second;
        return a.first < b.first;
    });
}

void CompetitionSimulator::printOutput() {
    LOG(LogLevel::INFO, "Starting to write formatted output to buffer");
    std::stringstream buffer;
    buffer << "game_maps_folder=" << mapsFolder << "\n";
    buffer << "game_manager=" << managerFileName << "\n";
    buffer << "\n";
    for(auto& nameScorePair : scores){
        buffer << nameScorePair.first << " " << nameScorePair.second << "\n";
    }
    writeResultsToFile(buffer, "competition_", algorithmsFolder);
}



void CompetitionSimulator::run() {
    LOG(LogLevel::INFO, "Starting competition simulator run");
    const GameManagerRegistrar& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    const AlgorithmRegistrar& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    const auto& gmEntry = gameManagerRegistrar[0];
    const size_t n = scores.size();
    const size_t k = maps.size();
    auto runWorker = [&]() {
        while (true) {
            auto [mapToRunIndex, algoToRun] = fetchIndicesToRun();
            if (mapToRunIndex >= k) break;
            size_t secondAlgoToRunIndex = (algoToRun+1+(mapToRunIndex%(n-1))) % n;

            const auto &algoEntryOne = algorithmRegistrar[algoToRun];
            const auto &algoEntryTwo = algorithmRegistrar[secondAlgoToRunIndex];
            const auto& mapToRun = maps[mapToRunIndex];

            auto player1 = algoEntryOne.createPlayer(1, mapToRun.cols, mapToRun.rows, mapToRun.maxSteps, mapToRun.numShells);
            auto player2 = algoEntryTwo.createPlayer(2, mapToRun.cols, mapToRun.rows, mapToRun.maxSteps, mapToRun.numShells);
            auto gm = gmEntry.createGameManager(verbose);
            try {
                LOG(LogLevel::INFO, std::string("Starting game between: ") + algoEntryOne.name() + " and " + algoEntryTwo.name() + " on map: " + mapToRun.mapFileName);
                GameResult result = gm.get()->run(mapToRun.cols, mapToRun.rows,
                                                  *mapToRun.view.get(), mapToRun.mapFileName,
                                                  mapToRun.maxSteps, mapToRun.numShells,
                                                  *player1.get(), algoEntryOne.name(),
                                                  *player2.get(), algoEntryTwo.name(),
                                                  algoEntryOne.getTankAlgorithmFactory(), algoEntryTwo.getTankAlgorithmFactory());
                LOG(LogLevel::INFO, std::string("Finished game between: ") + algoEntryOne.name() + " and " + algoEntryTwo.name() + " on map: " + mapToRun.mapFileName);
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
    LOG(LogLevel::INFO, std::string("Created ") + std::to_string(threadCount) + " threads");
    runWorker();
    for (auto& t : threads)
        t.join();
    sortScores();
    printOutput();
}