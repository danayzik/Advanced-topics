#pragma once
#include "AbstractGameManager.h"
#include <cassert>
#include <string>
#include <sstream>
#include "Exceptions.h"
class GameManagerRegistrar {
    class GameManagerEntry{
        std::string so_name;
        GameManagerFactory factory;

    public:
        GameManagerEntry(const std::string& so_name) : so_name(so_name) {}
        void setFactory(GameManagerFactory&& gameManagerFactory) {
            assert(factory == nullptr);
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

public:
    static GameManagerRegistrar& getGameManagerRegistrar(){return registrar;};
    void createGameManagerFactoryEntry(const std::string& name) {
        managers.emplace_back(name);
    }
    void addFactoryToLastEntry(GameManagerFactory&& factory) {
        managers.back().setFactory(std::move(factory));
    }


    void validateLastRegistration() {
        const auto& last = managers.back();
        if (!last.hasFactory()) {
            std::stringstream msg;
            msg << last.name()
                << ": has factory: " << last.hasFactory();
            throw BadRegistrationException(msg.str());
        }
    }

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