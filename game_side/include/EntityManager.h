#pragma once
#include "Entities.h"
#include <optional>
#include <memory>
#include <vector>
class MapLoader;
class GameMap;

class EntityManager{

private:
    size_t generations = 0;
    std::vector<std::optional<std::unique_ptr<GameEntity>>> entities;
    std::vector<size_t> freeIndicesList;
    std::optional<size_t> getFreeIndex();
    template<typename EntityType, typename... Args>
    const GameEntity& createEntity(Args&&... args);


public:
    [[nodiscard]] GameEntity& getEntity(size_t entityIndex);
    [[nodiscard]] const GameEntity& getEntity(size_t entityIndex) const;
    void destroyEntity(size_t entityIndex);
    void destroyEntity(GameEntity& entity);
    const GameEntity& createTank(size_t y, size_t x, Direction dir, int playerIndex, int tankIndex, size_t numShells);
    const GameEntity& createMine(size_t y, size_t x);
    const GameEntity& createWall(size_t y, size_t x);
    const GameEntity& createShell(size_t y, size_t x, Direction dir);
    [[nodiscard]] bool isTankAlive(size_t entityIndex) const;

    EntityManager() = default;
    ~EntityManager() = default;

    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
    EntityManager(EntityManager&&) = delete;
    EntityManager& operator=(EntityManager&&) = delete;

};