#pragma once
#include "renderer.h"
#include "SFML/Graphics.hpp"
#include <vector>

class SFMLRenderer : public Renderer {
private:
    unsigned int rows, cols;
    const unsigned int cellSize = 48;
    unsigned int screenWidth;
    unsigned int screenHeight;
    sf::Vector2u windowSize;
    sf::RenderWindow window;
    std::filesystem::path blueTankImagePath = "assets/blue_tank.png";
    std::filesystem::path greenTankImagePath = "assets/green_tank.png";
    std::filesystem::path mineImagePath = "assets/mine.png";
    std::filesystem::path wallImagePath = "assets/wall.png";
    std::filesystem::path crackedWallImagePath = "assets/cracked_wall.png";
    std::filesystem::path shellImagePath = "assets/shell.png";
    sf::Texture blueTankTexture;
    sf::Texture greenTankTexture;
    sf::Texture mineTexture;
    sf::Texture wallTexture;
    sf::Texture crackedWallTexture;
    sf::Texture shellTexture;
    void drawEntity(const GameEntity* entity);
    void drawCell(const Cell* cell);
    const sf::Texture& getTexture(const GameEntity* entity) const;

public:


    SFMLRenderer(unsigned int rows, unsigned int cols);
    void initialize() override;
    void drawGrid(const std::vector<std::vector<Cell>>& grid) override;

};
