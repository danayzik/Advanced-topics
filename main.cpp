#include "game_manager.h"
#include <iostream>



int main(int argc, char* argv[]){
    string mapFilePath = argv[1];
    auto playerOne = ChasingPlayer(1);
    auto playerTwo = PlayerV1(2);
    GameManager* gameManager = nullptr;
    try {
        gameManager = new GameManager(playerOne, playerTwo, mapFilePath);
    } catch (const std::runtime_error& e) {
        std::cerr << "Map loading error: " << e.what() << std::endl;
        return 1;
    }
    GameResult gameResult = gameManager->run();

}