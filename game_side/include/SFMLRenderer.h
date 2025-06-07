#pragma once
#include "Renderer.h"
#include "SFML/Graphics.hpp"
#include <vector>

class SFMLRenderer : public Renderer {
private:
    size_t rows, cols;
    const unsigned int cellSize = 48;
    unsigned int screenWidth;
    unsigned int screenHeight;
    sf::Vector2u windowSize;
    sf::RenderWindow window;
    std::filesystem::path mineImagePath = "game_side/assets/mine.png";
    std::filesystem::path wallImagePath = "game_side/assets/wall.png";
    std::filesystem::path crackedWallImagePath = "game_side/assets/cracked_wall.png";
    std::filesystem::path shellImagePath = "game_side/assets/shell.png";
    std::array<std::filesystem::path, 9> tanksImagePaths = {};
    std::array<sf::Texture, 9> tanksTextures = {};
    sf::Texture mineTexture;
    sf::Texture wallTexture;
    sf::Texture crackedWallTexture;
    sf::Texture shellTexture;
    void drawEntity(const GameEntity& entity);
    void drawCell(const Cell& cell, const EntityManager& entityManager);
    const sf::Texture& getTexture(const GameEntity& entity) const;

public:
    SFMLRenderer(size_t rows, size_t cols);
    void initialize() override;
    void drawGrid(const std::vector<std::vector<Cell>>& grid, const EntityManager& entityManager) override;
    ~SFMLRenderer() override = default;
    SFMLRenderer(const SFMLRenderer&) = delete;
    SFMLRenderer& operator=(const SFMLRenderer&) = delete;
    SFMLRenderer(SFMLRenderer&&) = delete;
    SFMLRenderer& operator=(SFMLRenderer&&) = delete;
};
