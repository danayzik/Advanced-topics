#include "EntityManager.h"
#include "Entities.h"

std::optional<size_t> EntityManager::getFreeIndex() {
    if(!freeIndicesList.empty()){
        size_t i = freeIndicesList.back();
        freeIndicesList.pop_back();
        return i;
    }
    return std::nullopt;
}

template<typename EntityType, typename... Args>
const GameEntity& EntityManager::createEntity(Args&&... args) {
    if (auto i = getFreeIndex()) {
        size_t id = i.value();
        entities[id] = std::make_unique<EntityType>(id, std::forward<Args>(args)...);
        return **(entities[id]);
    } else {
        size_t id = generations++;
        entities.push_back(std::make_unique<EntityType>(id, std::forward<Args>(args)...));
        return **(entities[id]);
    }
}

const GameEntity& EntityManager::createMine(size_t y, size_t x) {
    return createEntity<Mine>(y, x);
}

const GameEntity& EntityManager::createWall(size_t y, size_t x) {
    return createEntity<Wall>(y, x);
}

const GameEntity& EntityManager::createShell(size_t y, size_t x, Direction dir) {
    return createEntity<Shell>(y, x, dir);
}

const GameEntity& EntityManager::createTank(size_t y, size_t x, Direction dir, int playerIndex, int tankIndex, size_t numShells) {
    return createEntity<Tank>(y, x, dir, playerIndex, tankIndex, numShells);
}

GameEntity& EntityManager::getEntity(size_t entityIndex){
    auto& opt = entities.at(entityIndex);
    if (!opt || !*opt) {
        throw std::runtime_error("Entity does not exist");
    }
    return **opt;
}

const GameEntity &EntityManager::getEntity(size_t entityIndex) const {
    auto& opt = entities.at(entityIndex);
    if (!opt || !*opt) {
        throw std::runtime_error("Entity does not exist");
    }
    return **opt;
}

bool EntityManager::isTankAlive(size_t entityIndex) const{
    const auto& opt = entities.at(entityIndex);
    if (!opt || !*opt) {
        return false;
    }
    return (**opt).isTank();
}



void EntityManager::destroyEntity(size_t entityIndex) {
    entities.at(entityIndex) = std::nullopt;
    freeIndicesList.push_back(entityIndex);
}

void EntityManager::destroyEntity(GameEntity &entity) {
    destroyEntity(entity.getEntityId());
}