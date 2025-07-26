#pragma once
#include "AbstractGameManager.h"
#include <cassert>
#include <string>

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
    static GameManagerRegistrar& getGameManagerRegistrar();
    void createGameManagerFactoryEntry(const std::string& name) {
        managers.emplace_back(name);
    }
    void addFactoryToLastEntry(GameManagerFactory&& factory) {
        managers.back().setFactory(std::move(factory));
    }

    struct BadRegistrationException {
        std::string name;
        bool hasName, hasFactory;
        BadRegistrationException(std::string name, bool hasName, bool hasFactory)
            : name(name), hasName(hasName),
              hasFactory(hasFactory) {}
    };
    void validateLastRegistration() {
        const auto& last = managers.back();
        bool hasName = (last.name() != "");
        if(!hasName || !last.hasFactory() ) {
            throw BadRegistrationException(last.name(), hasName,
                                           last.hasFactory());
        }
    }
    void removeLast() {
        managers.pop_back();
    }
    auto begin() const {
        return managers.begin();
    }
    auto end() const {
        return managers.end();
    }
    std::size_t count() const { return managers.size(); }
    void clear() { managers.clear(); }
};