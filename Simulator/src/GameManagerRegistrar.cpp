#include "GameManagerRegistrar.h"

GameManagerRegistrar GameManagerRegistrar::registrar;


void GameManagerRegistrar::validateLastRegistration() {
    const auto& last = managers.back();
    if (!last.hasFactory()) {
        std::stringstream msg;
        msg << last.name()
            << ": has factory: " << last.hasFactory();
        throw BadRegistrationException(msg.str());
    }
}