#include "sfml_renderer.h"

void SFMLRenderer::initialize(int r, int c) {
    rows = r;
    cols = c;
    sf::Vector2<unsigned int> dimensions = sf::Vector2(cols * cellSize, rows * cellSize);
    window.create(sf::VideoMode(dimensions), "tank_game", sf::State::Windowed);
}

void SFMLRenderer::render() {
    window.clear();

    for (auto& wall : walls) window.draw(wall);
    for (auto& mine : mines) window.draw(mine);
    for (auto& tank : tanks) window.draw(tank);

    window.display();

}

void SFMLRenderer::drawWall(int y, int x) {
    sf::RectangleShape wall(sf::Vector2f(cellSize, cellSize));
    wall.setFillColor(sf::Color::Blue);
    sf::Vector2f position = sf::Vector2f(x * cellSize, y * cellSize);
    wall.setPosition(position);
    walls.push_back(wall);
}

void SFMLRenderer::drawMine(int y, int x) {
    sf::CircleShape mine(cellSize / 2.5);
    mine.setFillColor(sf::Color::Red);
    sf::Vector2f position = sf::Vector2f(x * cellSize, y * cellSize);
    mine.setPosition(position);
    mines.push_back(mine);
}

void SFMLRenderer::drawTank(int y, int x, int playerNumber) {
    sf::CircleShape tank(cellSize / 2.0);
    tank.setFillColor(playerNumber == 1 ? sf::Color::Green : sf::Color::Yellow);
    sf::Vector2f position = sf::Vector2f(x * cellSize, y * cellSize);
    tank.setPosition(position);
    tanks.push_back(tank);
}
