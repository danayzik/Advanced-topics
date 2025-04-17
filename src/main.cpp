#include "../include/game_manager.h"
#include <iostream>


int main(int argc, char* argv[]){
    if(argc != 2){
        std::cerr << "Invalid number of arguments" << std::endl;
        return 1;
    }
    string mapFilePath = argv[1];
    std::unique_ptr<Algorithm> chasingAlgo = std::make_unique<ChasingAlgorithm>();
    std::unique_ptr<Algorithm> simpleAlgo = std::make_unique<AlgorithmV1>();
    Player playerOne = Player(1, std::move(chasingAlgo));
    Player playerTwo = Player(2, std::move(simpleAlgo));
    GameManager* gameManager = nullptr;
    bool visuals = false;
#ifdef USE_SFML
    visuals = true;
#endif
    try {
        gameManager = new GameManager(playerOne, playerTwo, mapFilePath, visuals);
    } catch (const std::runtime_error& e) {
        std::cerr << "Map loading error: " << e.what() << std::endl;
        return 1;
    }
    gameManager->run();
    delete gameManager;
}


