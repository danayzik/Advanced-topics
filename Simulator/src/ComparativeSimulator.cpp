#include "ComparativeSimulator.h"
#include "AlgorithmRegistrar.h"
#include <filesystem>
#include "GameManagerRegistrar.h"
#include <algorithm>
#include <iostream>
#include "Exceptions.h"

namespace fs = std::filesystem;

void ComparativeSimulator::loadMapFile(const std::string &path) {
    mapFileName = fs::path(path).filename().string();
    mapInfo = MapLoader::getInstance().loadMap(path, errorBuffer);
    mapInfo.mapFileName = mapFileName;
}

void ComparativeSimulator::loadAlgorithms(const ParsedArguments &arguments) {
    LOG(LogLevel::INFO, "Attempting to load algorithms");
    const std::string& algo1 = arguments.algo1.value();
    const std::string& algo2 = arguments.algo2.value();
    const std::string algo1Name = fs::path(algo1).stem().string();
    const std::string algo2Name = fs::path(algo2).stem().string();
    AlgorithmRegistrar& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    algorithmRegistrar.createAlgorithmFactoryEntry(algo1Name);
    loadSO(algo1);
    algorithmRegistrar.validateLastRegistration();
    if(algo1 == algo2){
        LOG(LogLevel::WARN, "Attempted to load same algorithm twice");
        algorithmRegistrar.duplicateFirstEntry();
    }
    else{
        algorithmRegistrar.createAlgorithmFactoryEntry(algo2Name);
        loadSO(algo2);
        algorithmRegistrar.validateLastRegistration();
    }
}

void ComparativeSimulator::loadGameManagers() {
    LOG(LogLevel::INFO, std::string("Attempting to load game managers from: ") + gameManagersFolder);
    GameManagerRegistrar& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    for (const auto& entry : fs::directory_iterator(gameManagersFolder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            std::string fileName = entry.path().stem().string();
            std::string fullPathStr = entry.path().string();
            gameManagerRegistrar.createGameManagerFactoryEntry(fileName);
            try{
                loadSO(fullPathStr);
                gameManagerRegistrar.validateLastRegistration();
            }
            catch (const SharedObjectLoadingException& e) {
                gameManagerRegistrar.removeLast();
                std::cout << e.what() << std::endl;
            }
            catch (const BadRegistrationException& e) {
                gameManagerRegistrar.removeLast();
                errorBuffer << "Game manager: " << fullPathStr << " registration failed\n";
            }
        }
    }
    LOG(LogLevel::INFO, std::string("Successfully loaded ") + std::to_string(gameManagerRegistrar.count()) + " game managers");
    results.resize(gameManagerRegistrar.count());
}


void ComparativeSimulator::loadArguments(const ParsedArguments &arguments) {
    LOG(LogLevel::INFO, "Loading parsed arguments into the comparative simulator");
    Simulator::loadArguments(arguments);
    gameManagersFolder = arguments.managersFolder.value();
    loadMapFile(arguments.mapFileName.value());
    loadAlgorithms(arguments);
    loadGameManagers();
    LOG(LogLevel::INFO, "Finished loading parsed argument into the comparative simulator");
}

void ComparativeSimulator::storeGameResult(GameResult &&result, size_t storeIndex) {
    std::lock_guard<std::mutex>lock(storingResultsMutex);
    results[storeIndex] = std::move(result);
}

void ComparativeSimulator::groupResults() {
    LOG(LogLevel::INFO, "Grouping managers with the same results");
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        bool crashed = crashedManagersIndices.count(i) > 0;
        GameResultKey key  = {result, mapInfo.rows, mapInfo.cols, crashed};
        groups[key].push_back(i);
    }
    LOG(LogLevel::INFO, "Finished Grouping");
    LOG(LogLevel::INFO, std::string("Found ") + std::to_string(groups.size()) + " groups with different game results");
}

void ComparativeSimulator::printGameResult(const GameResult &result, std::stringstream& buffer) {
    LOG(LogLevel::INFO, "Writing info of game result to buffer");
    if (result.winner == 0){
        switch (result.reason) {
            case GameResult::Reason::MAX_STEPS:
                buffer << "Tie, reached max steps = " << mapInfo.maxSteps << ", ";
                buffer << "Player 1" << " has " << result.remaining_tanks[0] << " tanks, ";
                buffer << "Player 2" << " has " << result.remaining_tanks[1] << " tanks";
                break;
            case GameResult::Reason::ZERO_SHELLS:
                buffer << "Tie, both players have zero shells for 40 steps";
                break;
            case GameResult::Reason::ALL_TANKS_DEAD:
                buffer << "Tie, both players have zero tanks";
        }
    }
    else{
        buffer << "Player " << result.winner << " won with " << result.remaining_tanks[result.winner-1] << " tanks still alive";
    }
    buffer << "\n";
    buffer << result.rounds << "\n";
    LOG(LogLevel::INFO, "Finished writing");
}

void ComparativeSimulator::printOutput() {
    LOG(LogLevel::INFO, "Starting to write formatted output to buffer");
    std::vector<std::vector<size_t>> groupedIndices;
    for (auto& [_, vec] : groups)
        groupedIndices.push_back(vec);
    std::sort(groupedIndices.begin(), groupedIndices.end(),
              [](const std::vector<size_t>& a, const std::vector<size_t>& b) {
                  return a.size() > b.size();
              });
    AlgorithmRegistrar& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    assert(algorithmRegistrar.count() > 1);

    GameManagerRegistrar& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    std::stringstream buffer;

    buffer << "game_map=" << mapFileName << "\n";
    buffer << "algorithm1=" << algorithmRegistrar[0].name() << "\n";
    buffer << "algorithm2=" << algorithmRegistrar[1].name() << "\n";
    buffer << "\n";
    for(auto& indicesVec : groupedIndices){
        assert(!indicesVec.empty());
        for (size_t i = 0; i < indicesVec.size(); ++i) {
            size_t index = indicesVec[i];
            buffer << gameManagerRegistrar[index].name();
            if (i < indicesVec.size()-1) buffer << ", ";
        }
        buffer << "\n";
        size_t gmIndex = indicesVec.front();

        const GameResult& result = results[gmIndex];

        if (crashedManagersIndices.count(gmIndex) > 0){
            buffer << "These game managers crashed or returned an invalid result\n";
        }
        else{
            printGameResult(result, buffer);
            buffer << Map::getStringFromView(*result.gameState, mapInfo.rows, mapInfo.cols) << "\n";
        }
    }
    LOG(LogLevel::INFO, "Finished writing formatted output to buffer");
    writeResultsToFile(buffer, "comparative_results_", gameManagersFolder);
}





void ComparativeSimulator::run() {
    LOG(LogLevel::INFO, "Starting comparative simulator run");
    GameManagerRegistrar& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    AlgorithmRegistrar& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    const auto& algo1 = algorithmRegistrar[0];
    const auto& algo2 = algorithmRegistrar[1];
    const TankAlgorithmFactory& tankAlgorithmFactory1 = algo1.getTankAlgorithmFactory();
    const TankAlgorithmFactory& tankAlgorithmFactory2 = algo2.getTankAlgorithmFactory();

    auto runWorker = [&]() {
        while (true) {
            size_t i = nextIndex.fetch_add(1);
            if (i >= gameManagerRegistrar.count()) break;

            const auto& entry = gameManagerRegistrar[i];
            auto player1 = algo1.createPlayer(1, mapInfo.cols, mapInfo.rows, mapInfo.maxSteps, mapInfo.numShells);
            auto player2 = algo2.createPlayer(2, mapInfo.cols, mapInfo.rows, mapInfo.maxSteps, mapInfo.numShells);
            auto gm = entry.createGameManager(verbose);
            try {
                LOG(LogLevel::INFO, std::string("Starting game with game manager: ") + entry.name());
                GameResult result = gm.get()->run(mapInfo.cols, mapInfo.rows,
                                                  *mapInfo.view.get(), mapFileName,
                                                  mapInfo.maxSteps, mapInfo.numShells,
                                                  *player1.get(), algo1.name(),
                                                  *player2.get(), algo2.name(),
                                                  tankAlgorithmFactory1, tankAlgorithmFactory2);
                LOG(LogLevel::INFO, std::string("Finished run with game manager: ") + entry.name());
                if (validGameResult(result)) {
                    storeGameResult(std::move(result), i);
                }
                else{
                    std::lock_guard<std::mutex> lock(errorMutex);
                    errorBuffer << "Game manager: " << entry.name() << " returned an invalid result\n";
                    crashedManagersIndices.insert(i);
                }


            }
            catch (...) {
                std::lock_guard<std::mutex> lock(errorMutex);
                errorBuffer << "Game manager: " << entry.name() << " crashed\n";
                crashedManagersIndices.insert(i);
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
    groupResults();
    printOutput();
}



