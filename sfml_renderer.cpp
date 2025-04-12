#include "sfml_renderer.h"

SFMLRenderer::SFMLRenderer(unsigned int rows, unsigned int cols)  : rows(rows), cols(cols), screenWidth(cols*cellSize) , screenHeight(rows * cellSize){
    windowSize = {screenWidth, screenHeight};}

void SFMLRenderer::initialize() {
    window.create(sf::VideoMode(windowSize), "tank_game", sf::State::Windowed);
}
std::filesystem::path SFMLRenderer::getImagePath (const GameEntity* entity) const{
    std::filesystem::path imagePath;
    if(entity->isMine())
        imagePath = mineImagePath;
    if(entity->isWall()) {
        if (entity->getHealth() == 1) {
            imagePath = crackedWallImagePath;
        } else {
            imagePath = wallImagePath;
        }

    }
    if(entity->isShell()){
        imagePath = shellImagePath;
    }
    if(entity->isTank()){
        if(entity->getOwner() == PlayerOne){
            imagePath = blueTankImagePath;
        }
        if(entity->getOwner() == PlayerTwo){
            imagePath = greenTankImagePath;
        }
    }
    return imagePath;
}

void SFMLRenderer::drawEntity(const GameEntity* entity){
    sf::Texture texture;
    if(!texture.loadFromFile(getImagePath(entity))){
        //Throw error
    }
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


void SFMLRenderer::drawCell(const Cell* cell, int y, int x){
    sf::RectangleShape square(sf::Vector2f(cellSize, cellSize));
    float xPos = x*cellSize;
    float yPos = y*cellSize;
    square.setPosition({xPos, yPos});
    square.setFillColor(sf::Color::Black);
    window.draw(square);
    for(const GameEntity* entity : cell->entitySet){
        drawEntity(entity);
    }
}

//void SFMLRenderer::drawDirtyCells(const std::unordered_set<Cell*>)

void SFMLRenderer::drawGrid(const std::vector<std::vector<Cell>>& grid){
    for (std::size_t i = 0; i < grid.size(); ++i) {
        for (std::size_t j = 0; j < grid[i].size(); ++j) {
            drawCell(&grid[i][j], i, j);
        }
    }
    window.display();
}
