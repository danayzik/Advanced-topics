#include "../include/game_manager.h"
#include <iostream>
#include "SFML/Graphics.hpp"


int main(int argc, char* argv[]){
    string mapFilePath = argv[1];
    std::unique_ptr<Algorithm> chasingAlgo = std::make_unique<ChasingAlgorithm>();
    std::unique_ptr<Algorithm> simpleAlgo = std::make_unique<AlgorithmV1>();
    Player playerOne = Player(1, std::move(chasingAlgo));
    Player playerTwo = Player(2, std::move(simpleAlgo));
    GameManager* gameManager = nullptr;
    try {
        gameManager = new GameManager(playerOne, playerTwo, mapFilePath, true);
    } catch (const std::runtime_error& e) {
        std::cerr << "Map loading error: " << e.what() << std::endl;
        return 1;
    }
    GameResult gameResult = gameManager->run();
    delete gameManager;
}


