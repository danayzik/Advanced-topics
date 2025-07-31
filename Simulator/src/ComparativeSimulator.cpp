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
    mapInfo = std::move(MapLoader::getInstance().loadMap(path, errorBuffer));
}

void ComparativeSimulator::loadAlgorithms(const ParsedArguments &arguments) {
    const std::string& algo1 = arguments.algo1.value();
    const std::string& algo2 = arguments.algo2.value();
    const std::string algo1Name = fs::path(algo1).stem().string();
    const std::string algo2Name = fs::path(algo2).stem().string();
    AlgorithmRegistrar& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    algorithmRegistrar.createAlgorithmFactoryEntry(algo1Name);
    loadSO(algo1);
    algorithmRegistrar.validateLastRegistration();
    if(algo1 == algo2){
        algorithmRegistrar.duplicateFirstEntry();
    }
    else{
        algorithmRegistrar.createAlgorithmFactoryEntry(algo2Name);
        loadSO(algo2);
        algorithmRegistrar.validateLastRegistration();
    }
}

void ComparativeSimulator::loadGameManagers() {
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
                std::cout << e.what() << std::endl;
            }
            catch (const BadRegistrationException& e) {
                gameManagerRegistrar.removeLast();
                errorBuffer << "Game manager: " << fullPathStr <<" registration failed\n";
            }
        }
    }
    results.resize(gameManagerRegistrar.count());
}


void ComparativeSimulator::loadArguments(const ParsedArguments &arguments) {
    Simulator::loadArguments(arguments);
    gameManagersFolder = arguments.managersFolder.value();
    loadMapFile(arguments.mapFileName.value());
    loadAlgorithms(arguments);
    loadGameManagers();
}

void ComparativeSimulator::storeGameResult(GameResult &&result, size_t storeIndex) {
    results[storeIndex] = std::move(result);
}

void ComparativeSimulator::groupResults() {
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        bool crashed = crashedManagersIndices.count(i) > 0;
        GameResultKey key  = {result, mapInfo.rows, mapInfo.cols, crashed};
        groups[key].push_back(i);
    }
}

void ComparativeSimulator::printGameResult(const GameResult &result, std::stringstream& buffer) {
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

}

void ComparativeSimulator::printOutput() {
    std::vector<std::vector<size_t>> groupedIndices;

    for (auto& [_, vec] : groups)
        groupedIndices.push_back(vec);
    std::sort(groupedIndices.begin(), groupedIndices.end(),
              [](const std::vector<size_t>& a, const std::vector<size_t>& b) {
                  return a.size() > b.size();
              });
    AlgorithmRegistrar& algorithmRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    GameManagerRegistrar& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
    std::stringstream buffer;

    buffer << mapFileName << "\n";
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
            buffer << "These game managers crashed\n";
        }
        else{
            printGameResult(result, buffer);

            buffer << Map::getStringFromView(*result.gameState, mapInfo.rows, mapInfo.cols);
        }
    }
    writeResultsToFile(buffer);
}

void ComparativeSimulator::writeResultsToFile(const std::stringstream &ss) {
    std::string timeStr = getTimeString();
    std::string filename = "comparative_results_" + timeStr + ".txt";
    std::filesystem::path filePath = std::filesystem::path(gameManagersFolder) / filename;

    std::ofstream outFile(filePath);
    if (!outFile) {
        throw std::runtime_error("Failed to open file: " + filePath.string());
    }
    outFile << ss.str();
}



void ComparativeSimulator::run() {
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
                GameResult result = gm.get()->run(mapInfo.cols, mapInfo.rows,
                                                  *mapInfo.view.get(), mapFileName,
                                                  mapInfo.maxSteps, mapInfo.numShells,
                                                  *player1.get(), algo1.name(),
                                                  *player2.get(), algo2.name(),
                                                  tankAlgorithmFactory1, tankAlgorithmFactory2);
                storeGameResult(std::move(result), i);
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

    runWorker();
    for (auto& t : threads)
        t.join();
    groupResults();
    printOutput();
}



