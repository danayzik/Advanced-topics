#pragma once
#include "AbstractGameManager.h"
#include <cassert>
#include <string>
#include <sstream>
#include "Exceptions.h"
#include <iostream>
#include "Logger.h"
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
                LOG(LogLevel::WARN, std::string("Attempted Game manager registration for so: ") + so_name + "\nThis factory was already registered.");
                return;
            }
            LOG(LogLevel::INFO, "Registering game manager factory for so: " + so_name);
            factory = std::move(gameManagerFactory);
        }
        const std::string& name() const { return so_name; }
        std::unique_ptr<AbstractGameManager> createGameManager(bool verbose) const {
            LOG(LogLevel::INFO, std::string("Creating game manager from so: ") + so_name);
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
        LOG(LogLevel::INFO, std::string("Creating a spot for a factory from so: ") + name);
        managers.emplace_back(name);
    }
    void addFactoryToLastEntry(GameManagerFactory&& factory) {
        managers.back().setFactory(std::move(factory));
    }

    void validateLastRegistration();


    void removeLast() {
        LOG(LogLevel::INFO, "Removing last Game manager factory entry");
        managers.pop_back();
    }

    std::size_t count() const { return managers.size(); }
    void clear() {
        LOG(LogLevel::INFO, "Clearing Game manager registrar");
        managers.clear();
    }

    GameManagerEntry& operator[](size_t i){
        return managers[i];
    }
    const GameManagerEntry& operator[](size_t i) const{
        return managers[i];
    }
};