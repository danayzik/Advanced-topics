#include "SFMLRenderer.h"
#include "EntityManager.h"
using namespace DirectionUtils;


SFMLRenderer::SFMLRenderer(size_t rows, size_t cols)  : rows(rows), cols(cols), screenWidth(cols*cellSize) , screenHeight(rows * cellSize){
    windowSize = {screenWidth, screenHeight};}

void SFMLRenderer::initialize() {
    window.create(sf::VideoMode(windowSize), "tank_game", sf::State::Windowed);

    if (!mineTexture.loadFromFile(mineImagePath))
        throw std::runtime_error("Failed to fetch image: " + mineImagePath.string());

    if (!wallTexture.loadFromFile(wallImagePath))
        throw std::runtime_error("Failed to fetch image: " + wallImagePath.string());

    if (!crackedWallTexture.loadFromFile(crackedWallImagePath))
        throw std::runtime_error("Failed to fetch image: " + crackedWallImagePath.string());

    if (!shellTexture.loadFromFile(shellImagePath))
        throw std::runtime_error("Failed to fetch image: " + shellImagePath.string());

    for(size_t i = 0; i < tanksTextures.size(); i++){
        std::string filename = "game_side/assets/tank_" + std::to_string(i + 1) + ".png";
        tanksImagePaths[i] = std::filesystem::path(filename);
        if(!tanksTextures[i].loadFromFile(tanksImagePaths[i]))
            throw std::runtime_error("Failed to fetch image: " + tanksImagePaths[i].string());
    }
}

const sf::Texture& SFMLRenderer::getTexture(const GameEntity& entity) const{
    if(entity.isMine())
        return mineTexture;
    if(entity.isWall()) {
        if (entity.getHealth() == 1) {
            return crackedWallTexture;
        } else {
            return wallTexture;
        }
    }
    if(entity.isShell()){
        return shellTexture;
    }
    if(entity.isTank()){
        int playerIndex = dynamic_cast<const Tank&>(entity).getPlayerIndex();
        return tanksTextures[playerIndex-1];
    }
    return shellTexture; //Default, need to add error image
}

void SFMLRenderer::drawEntity(const GameEntity& entity){
    const sf::Texture& texture = getTexture(entity);
    sf::Sprite sprite(texture);
    sf::Vector2u originalSize = texture.getSize();
    auto targetWidth = static_cast<float>(cellSize);
    auto targetHeight = static_cast<float>(cellSize);
    sprite.setScale(
            {targetWidth / static_cast<float>(originalSize.x),
            targetHeight / static_cast<float>(originalSize.y)}
    );
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    if(entity.isTank()){
        Direction dir = dynamic_cast<const Tank&>(entity).getDirection();
        auto angle = static_cast<float>(((directionToInt(dir) + 90) % 360));
        sprite.rotate(sf::degrees(angle));
    }
    auto x = static_cast<float>(entity.getX() * cellSize) + targetWidth / 2.f;
    auto y = static_cast<float>(entity.getY() * cellSize) + targetHeight / 2.f;
    sprite.setPosition({x, y});
    window.draw(sprite);
}


void SFMLRenderer::drawCell(const Cell& cell, const EntityManager& entityManager){
    sf::RectangleShape square(sf::Vector2f(static_cast<float>(cellSize), static_cast<float>(cellSize)));
    auto xPos = static_cast<float>(cell.x()*cellSize);
    auto yPos = static_cast<float>(cell.y()*cellSize);
    square.setPosition({xPos, yPos});
    square.setFillColor(sf::Color::Black);
    window.draw(square);
    for(auto entityId : cell.entitySet){
        drawEntity(entityManager.getEntity(entityId));
    }
}


void SFMLRenderer::drawGrid(const std::vector<std::vector<Cell>>& grid, const EntityManager& entityManager){
    window.clear();
    for (const auto & row : grid) {
        for (const auto & cell : row) {
            drawCell(cell, entityManager);
        }
    }
    window.display();
}
