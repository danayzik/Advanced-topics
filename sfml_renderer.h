#pragma once
#include "renderer.h"
#include <SFML/Graphics.hpp>
#include <vector>

class SFMLRenderer : public Renderer {
private:
    unsigned int rows, cols;
    const unsigned int cellSize = 32;
    unsigned int screenWidth;
    unsigned int screenHeight;
    sf::Vector2u windowSize;
    sf::RenderWindow window;
    std::filesystem::path blueTankImagePath = "C:\\Users\\danay\\CLionProjects\\Advanced-topics\\assets\\blue_tank.png";
    std::filesystem::path greenTankImagePath = "C:\\Users\\danay\\CLionProjects\\Advanced-topics\\assets\\green_tank.png";
    std::filesystem::path mineImagePath = "C:\\Users\\danay\\CLionProjects\\Advanced-topics\\assets\\mine.png";
    std::filesystem::path wallImagePath = "C:\\Users\\danay\\CLionProjects\\Advanced-topics\\assets\\wall.png";
    std::filesystem::path crackedWallImagePath = "C:\\Users\\danay\\CLionProjects\\Advanced-topics\\assets\\cracked_wall.png";
    std::filesystem::path shellImagePath = "C:\\Users\\danay\\CLionProjects\\Advanced-topics\\assets\\shell.png";

public:
    std::filesystem::path getImagePath(const GameEntity* entity) const;
    void drawEntity(const GameEntity* entity);
    SFMLRenderer(unsigned int rows, unsigned int cols);
    void initialize() override;
    void drawGrid(const std::vector<std::vector<Cell>>& grid) override;
    void drawCell(const Cell* cell, int y, int x) override;
};
