#include "sfml_renderer.h"

SFMLRenderer::SFMLRenderer(unsigned int rows, unsigned int cols)  : rows(rows), cols(cols), screenWidth(cols*cellSize) , screenHeight(rows * cellSize){
    windowSize = {screenWidth, screenHeight};}

void SFMLRenderer::initialize() {
    window.create(sf::VideoMode(windowSize), "tank_game", sf::State::Windowed);
}

void SFMLRenderer::drawEntity(GameEntity* entity){

}

void SFMLRenderer::drawWall(int y, int x) {
    sf::Texture texture(wallImagePath);
    sf::Sprite sprite(texture);
    sf::Vector2u originalSize = texture.getSize();
    float targetWidth = 32.f;
    float targetHeight = 32.f;
    sprite.setScale(
            {targetWidth / originalSize.x,
            targetHeight / originalSize.y}
    );
    sf::Vector2f position = sf::Vector2f(x * cellSize, y * cellSize);
    sprite.setPosition(position);
    window.draw(sprite);
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

void SFMLRenderer::drawCell(const Cell* cell, int y, int x){
    return;
}
