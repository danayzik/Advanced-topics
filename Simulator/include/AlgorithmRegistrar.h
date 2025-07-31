#pragma once
#include "TankAlgorithm.h"
#include "Player.h"
#include <cassert>
#include <string>
#include "Exceptions.h"
#include <sstream>




class AlgorithmRegistrar {
    class AlgorithmAndPlayerFactories {
        std::string so_name;
        TankAlgorithmFactory tankAlgorithmFactory;
        PlayerFactory playerFactory;
    public:
        AlgorithmAndPlayerFactories(const std::string& so_name) : so_name(so_name) {}
        AlgorithmAndPlayerFactories(const AlgorithmAndPlayerFactories& other)
                : so_name(other.so_name)
        {
            if (other.playerFactory)
                playerFactory = other.playerFactory;
            if (other.tankAlgorithmFactory)
                tankAlgorithmFactory = other.tankAlgorithmFactory;
        }
        void setTankAlgorithmFactory(TankAlgorithmFactory&& factory) {
            assert(tankAlgorithmFactory == nullptr);
            tankAlgorithmFactory = std::move(factory);
        }
        void setPlayerFactory(PlayerFactory&& factory) {
            assert(playerFactory == nullptr);
            playerFactory = std::move(factory);
        }
        const std::string& name() const { return so_name; }
        std::unique_ptr<Player> createPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) const {
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
    AlgorithmRegistrar(const AlgorithmRegistrar&) = delete;
    AlgorithmRegistrar& operator=(const AlgorithmRegistrar&) = delete;
    std::vector<AlgorithmAndPlayerFactories> algorithms;
    static AlgorithmRegistrar registrar;
public:



    static AlgorithmRegistrar& getAlgorithmRegistrar();
    void createAlgorithmFactoryEntry(const std::string& name) {
        algorithms.emplace_back(name);
    }
    void addPlayerFactoryToLastEntry(PlayerFactory&& factory) {
        algorithms.back().setPlayerFactory(std::move(factory));
    }
    void addTankAlgorithmFactoryToLastEntry(TankAlgorithmFactory&& factory) {
        algorithms.back().setTankAlgorithmFactory(std::move(factory));
    }

    void validateLastRegistration() {
        const auto& last = algorithms.back();
        if (!last.hasPlayerFactory() || !last.hasTankAlgorithmFactory()) {
            std::stringstream msg;
            msg << last.name()
                << ": has player factory: " << last.hasPlayerFactory()
                << ", has tank algorithm factory: " << last.hasTankAlgorithmFactory();
            throw BadRegistrationException(msg.str());
        }
    }
    void removeLast() {
        algorithms.pop_back();
    }

    void duplicateFirstEntry() {
        if (algorithms.empty()) return;
        AlgorithmAndPlayerFactories copy(algorithms.front());
        algorithms.push_back(std::move(copy));
    }
    std::size_t count() const { return algorithms.size(); }
    void clear() { algorithms.clear(); }
    AlgorithmAndPlayerFactories& operator[](size_t i){
        return algorithms[i];
    }
    const AlgorithmAndPlayerFactories& operator[](size_t i) const{
        return algorithms[i];
    }
};