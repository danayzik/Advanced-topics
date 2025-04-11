#pragma once
#include "renderer.h"
#include <SFML/Graphics.hpp>
#include <vector>

class SFMLRenderer : public Renderer {
private:
    unsigned int rows, cols;
    unsigned int cellSize = 32;
    sf::RenderWindow window;
    std::vector<sf::RectangleShape> walls;
    std::vector<sf::CircleShape> mines;
    std::vector<sf::CircleShape> tanks;

public:
    void initialize(int rows, int cols) override;
    void render() override;
    void drawTank(int y, int x, int playerNumber) override;
    void drawWall(int y, int x) override;
    void drawMine(int y, int x) override;
};
