#include "GameManagerRegistrar.h"
#include "Logger.h"
GameManagerRegistrar GameManagerRegistrar::registrar;


void GameManagerRegistrar::validateLastRegistration() {
    const auto& last = managers.back();
    if (!last.hasFactory()) {
        std::stringstream msg;
        msg << last.name()
            << ": has factory: " << last.hasFactory();
        throw BadRegistrationException(msg.str());
    }
    LOG(LogLevel::INFO, std::string("Successfully validated registration for ") + last.name());
}