#pragma once
#include "AbstractGameManager.h"
#include <cassert>
#include <string>
#include <sstream>
#include "Exceptions.h"
#include <iostream>
class GameManagerRegistrar {
private:
    class GameManagerEntry{
        std::string so_name;
        GameManagerFactory factory;

    public:
        GameManagerEntry(const std::string& so_name) : so_name(so_name) {}
        void setFactory(GameManagerFactory&& gameManagerFactory) {
            if(factory){
                std::cout << "Duplicate registration attempted, ignoring\n";
                return;
            }
            factory = std::move(gameManagerFactory);
        }
        const std::string& name() const { return so_name; }
        std::unique_ptr<AbstractGameManager> createGameManager(bool verbose) const {
            return factory(verbose);
        }
        bool hasFactory() const {
            return factory != nullptr;
        }

    };
    std::vector<GameManagerEntry> managers;
    static GameManagerRegistrar registrar;
    GameManagerRegistrar() = default;

public:
    GameManagerRegistrar(const GameManagerRegistrar&) = delete;
    GameManagerRegistrar& operator=(const GameManagerRegistrar&) = delete;
    GameManagerRegistrar(GameManagerRegistrar&&) = delete;
    GameManagerRegistrar& operator=(GameManagerRegistrar&&) = delete;
    ~GameManagerRegistrar() = default;

    static GameManagerRegistrar& getGameManagerRegistrar(){return registrar;};

    void createGameManagerFactoryEntry(const std::string& name) {
        managers.emplace_back(name);
    }
    void addFactoryToLastEntry(GameManagerFactory&& factory) {
        managers.back().setFactory(std::move(factory));
    }

    void validateLastRegistration();


    void removeLast() {
        managers.pop_back();
    }

    std::size_t count() const { return managers.size(); }
    void clear() { managers.clear(); }

    GameManagerEntry& operator[](size_t i){
        return managers[i];
    }
    const GameManagerEntry& operator[](size_t i) const{
        return managers[i];
    }
};