#include "ComparativeSimulator.h"
#include "AlgorithmRegistrar.h"
#include <filesystem>
#include "GameManagerRegistrar.h"
#include <algorithm>
#include <iostream>

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
    algorithmRegistrar.validateLastRegistration();
    if(algo1 == algo2){
        algorithmRegistrar.duplicateFirstEntry();
    }
    else{
        algorithmRegistrar.createAlgorithmFactoryEntry(algo2Name);
        loadSO(algo2);
        algorithmRegistrar.validateLastRegistration();
    }


    mapFileName = fs::path(mapPath).filename().string();

    mapInfo = std::move(MapLoader::getInstance().loadMap(mapPath, errorBuffer));

    verbose = arguments.verbose;
    threadCount = arguments.numThreads >= 2 ? arguments.numThreads : 0;

    gameManagersFolder = arguments.managersFolder.value();


    for (const auto& entry : fs::directory_iterator(gameManagersFolder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            std::string fileName = entry.path().filename().string();
            std::string fullPathStr = entry.path().string();
            gameManagerRegistrar.createGameManagerFactoryEntry(fileName);
            try{
                loadSO(fullPathStr);
                gameManagerRegistrar.validateLastRegistration();
            }
            catch (const std::runtime_error& e) {
                std::cout << e.what() << std::endl;
            }
            catch (const AlgorithmRegistrar::BadRegistrationException& e) {
                gameManagerRegistrar.removeLast();
                errorBuffer << "Game manager: " << fullPathStr <<" registration failed\n";
            }


        }
    }

    results.resize(gameManagerRegistrar.count());

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
    if (result.winner != 0){
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
        std::string finalView;
        if (crashedManagersIndices.count(gmIndex) > 0){
            buffer << "These game managers crashed\n";
        }
        else{
            printGameResult(result, buffer);
            finalView.reserve((mapInfo.cols + 1) * mapInfo.rows);
            for (size_t y = 0; y < mapInfo.rows; ++y) {
                for (size_t x = 0; x < mapInfo.cols; ++x) {
                    finalView += result.gameState->getObjectAt(x, y);
                }
                finalView += '\n';
            }
            buffer << finalView;
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
    const AlgorithmAndPlayerFactories& algo1 = algorithmRegistrar[0];
    const AlgorithmAndPlayerFactories& algo2 = algorithmRegistrar[1];
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


ComparativeSimulator::~ComparativeSimulator(){
    if(errorBuffer.str().empty()){
       return;
    }
    std::string fileName = "input_errors.txt";
    std::ofstream outFile(fileName);
    if (!outFile) {
        std::cerr << "Failed to open file: " << fileName << '\n';
        return;
    }
    outFile << errorBuffer.str();
}
