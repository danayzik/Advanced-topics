#include "game_manager.h"
#include <iostream>
#include "SFML/Graphics.hpp"

//
//int main(int argc, char* argv[]){
//    string mapFilePath = argv[1];
//    auto playerOne = ChasingPlayer(1);
//    auto playerTwo = PlayerV1(2);
//    GameManager* gameManager = nullptr;
//    try {
//        gameManager = new GameManager(playerOne, playerTwo, mapFilePath);
//    } catch (const std::runtime_error& e) {
//        std::cerr << "Map loading error: " << e.what() << std::endl;
//        return 1;
//    }
//    GameResult gameResult = gameManager->run();
//
//}

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1080u, 1080u}), "CMake SFML Project");
    window.setFramerateLimit(144);
    sf::Texture texture;
    if (!texture.loadFromFile("C:\\Users\\danay\\CLionProjects\\Advanced-topics\\assets\\blue_tank.png")) {
        return -1; // handle error
    }

    sf::Sprite sprite(texture);
    sf::Vector2u originalSize = texture.getSize();
    std::cout << originalSize.x << " " << originalSize.y;
    // Resize the image (scale factors relative to original size)
    sprite.setScale({0.5f, 0.5f});
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
}
