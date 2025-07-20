#include "GameManager.h"
#include <iostream>

using namespace GameManager_206038929_314620071;
int main(int argc, char** argv){
    if(argc != 2){
        std::cerr << "Invalid number of arguments" << std::endl;
        return 1;
    }
    string mapFilePath = argv[1];
    MyPlayerFactory pf;
    MyTankAlgorithmFactory taf;
    GameManager gameManager(pf, taf);
    try {
        gameManager.readBoard(mapFilePath);
    } catch (const std::runtime_error& e) {
        std::cerr << "Map loading error: " << e.what() << std::endl;
        return 1;
    }
    gameManager.run();
}


