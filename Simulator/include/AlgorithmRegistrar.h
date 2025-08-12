#pragma once
#include "TankAlgorithm.h"
#include "Player.h"
#include <cassert>
#include <string>
#include "Exceptions.h"
#include <sstream>
#include <iostream>
#include "Logger.h"


class AlgorithmRegistrar {
    class AlgorithmAndPlayerFactories {
        std::string so_name;
        TankAlgorithmFactory tankAlgorithmFactory;
        PlayerFactory playerFactory;
    public:
        AlgorithmAndPlayerFactories& operator=(const AlgorithmAndPlayerFactories&) = default;
        AlgorithmAndPlayerFactories(AlgorithmAndPlayerFactories&&) = default;
        AlgorithmAndPlayerFactories& operator=(AlgorithmAndPlayerFactories&&) = default;
        AlgorithmAndPlayerFactories(const AlgorithmAndPlayerFactories& other) = default;
        ~AlgorithmAndPlayerFactories() = default;

        AlgorithmAndPlayerFactories(const std::string& so_name) : so_name(so_name) {}

        void setTankAlgorithmFactory(TankAlgorithmFactory&& factory) {
            if(tankAlgorithmFactory){
                std::cout << "Duplicate registration attempted, ignoring\n";
                LOG(LogLevel::WARN, std::string("Attempted algorithm factory registration for so: ") + so_name + "\nThis factory was already registered.");
                return;
            }
            LOG(LogLevel::INFO, "Registering algorithm factory for so: " + so_name);
            tankAlgorithmFactory = std::move(factory);
        }
        void setPlayerFactory(PlayerFactory&& factory) {
            if(playerFactory){
                std::cout << "Duplicate registration attempted, ignoring\n";
                LOG(LogLevel::WARN, std::string("Attempted player factory registration for so: ") + so_name + "\nThis factory was already registered.");
                return;
            }
            LOG(LogLevel::INFO, std::string("Registering player factory for so: ") + so_name);
            playerFactory = std::move(factory);
        }
        const std::string& name() const { return so_name; }
        std::unique_ptr<Player> createPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) const {
            LOG(LogLevel::INFO, std::string("Creating player from so: ") + so_name);
            return playerFactory(player_index, x, y, max_steps, num_shells);
        }

        bool hasPlayerFactory() const {
            return playerFactory != nullptr;
        }
        bool hasTankAlgorithmFactory() const {
            return tankAlgorithmFactory != nullptr;
        }
        const TankAlgorithmFactory& getTankAlgorithmFactory() const{ return tankAlgorithmFactory;}
    };

private:
    AlgorithmRegistrar() = default;
    std::vector<AlgorithmAndPlayerFactories> algorithms;
    static AlgorithmRegistrar registrar;
public:

    AlgorithmRegistrar(const AlgorithmRegistrar&) = delete;
    AlgorithmRegistrar& operator=(const AlgorithmRegistrar&) = delete;
    AlgorithmRegistrar(AlgorithmRegistrar&&) = delete;
    AlgorithmRegistrar& operator=(AlgorithmRegistrar&&) = delete;
    ~AlgorithmRegistrar() = default;

    static AlgorithmRegistrar& getAlgorithmRegistrar(){return registrar;};
    void createAlgorithmFactoryEntry(const std::string& name) {
        LOG(LogLevel::INFO, std::string("Creating a spot for factories from so: ") + name);
        algorithms.emplace_back(name);
    }
    void addPlayerFactoryToLastEntry(PlayerFactory&& factory) {
        algorithms.back().setPlayerFactory(std::move(factory));
    }
    void addTankAlgorithmFactoryToLastEntry(TankAlgorithmFactory&& factory) {
        algorithms.back().setTankAlgorithmFactory(std::move(factory));
    }

    void validateLastRegistration();

    void removeLast() {
        LOG(LogLevel::INFO, "Removing last Tank&Player factories entry");
        algorithms.pop_back();
    }

    void duplicateFirstEntry();


    std::size_t count() const { return algorithms.size(); }
    void clear() {
        LOG(LogLevel::INFO, "Clearing Algorithm registrar");
        algorithms.clear();
    }
    AlgorithmAndPlayerFactories& operator[](size_t i){
        return algorithms[i];
    }
    const AlgorithmAndPlayerFactories& operator[](size_t i) const{
        return algorithms[i];
    }
};