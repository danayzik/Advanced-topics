#include "../include/sfml_renderer.h"

SFMLRenderer::SFMLRenderer(unsigned int rows, unsigned int cols)  : rows(rows), cols(cols), screenWidth(cols*cellSize) , screenHeight(rows * cellSize){
    windowSize = {screenWidth, screenHeight};}

void SFMLRenderer::initialize() {
    window.create(sf::VideoMode(windowSize), "tank_game", sf::State::Windowed);

    if (!blueTankTexture.loadFromFile(blueTankImagePath))
        throw std::runtime_error("Failed to fetch image: " + blueTankImagePath.string());

    if (!greenTankTexture.loadFromFile(greenTankImagePath))
        throw std::runtime_error("Failed to fetch image: " + greenTankImagePath.string());

    if (!mineTexture.loadFromFile(mineImagePath))
        throw std::runtime_error("Failed to fetch image: " + mineImagePath.string());

    if (!wallTexture.loadFromFile(wallImagePath))
        throw std::runtime_error("Failed to fetch image: " + wallImagePath.string());

    if (!crackedWallTexture.loadFromFile(crackedWallImagePath))
        throw std::runtime_error("Failed to fetch image: " + crackedWallImagePath.string());

    if (!shellTexture.loadFromFile(shellImagePath))
        throw std::runtime_error("Failed to fetch image: " + shellImagePath.string());

}
const sf::Texture& SFMLRenderer::getTexture(const GameEntity* entity) const{
    if(entity->isMine())
        return mineTexture;
    if(entity->isWall()) {
        if (entity->getHealth() == 1) {
            return crackedWallTexture;
        } else {
            return wallTexture;
        }
    }
    if(entity->isShell()){
        return shellTexture;
    }
    if(entity->isTank()){
        if(entity->getOwner() == PlayerOne){
            return blueTankTexture;
        }
        if(entity->getOwner() == PlayerTwo){
            return greenTankTexture;
        }
    }
    return shellTexture; //Default, need to add error image
}

void SFMLRenderer::drawEntity(const GameEntity* entity){
    const sf::Texture& texture = getTexture(entity);
    sf::Sprite sprite(texture);
    sf::Vector2u originalSize = texture.getSize();
    float targetWidth = cellSize;
    float targetHeight = cellSize;
    sprite.setScale(
            {targetWidth / originalSize.x,
            targetHeight / originalSize.y}
    );
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    if(entity->isTank()){
        Direction dir = dynamic_cast<const Tank*>(entity)->getDirection();
        float angle = ((dir + 90) % 360);
        sprite.rotate(sf::degrees(angle));
    }
    float x = entity->getX() * cellSize + cellSize / 2.f;
    float y = entity->getY() * cellSize + cellSize / 2.f;
    sprite.setPosition({x, y});
    window.draw(sprite);
}


void SFMLRenderer::drawCell(const Cell* cell){
    sf::RectangleShape square(sf::Vector2f(static_cast<float>(cellSize), static_cast<float>(cellSize)));
    auto xPos = static_cast<float>(cell->x*cellSize);
    auto yPos = static_cast<float>(cell->y*cellSize);
    square.setPosition({xPos, yPos});
    square.setFillColor(sf::Color::Black);
    window.draw(square);
    for(const GameEntity* entity : cell->entitySet){
        drawEntity(entity);
    }
}


void SFMLRenderer::drawGrid(const std::vector<std::vector<Cell>>& grid){
    window.clear();
    for (const auto & row : grid) {
        for (const auto & cell : row) {
            drawCell(&cell);
        }
    }
    window.display();
}
