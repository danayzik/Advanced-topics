#include "AlgorithmRegistrar.h"


AlgorithmRegistrar AlgorithmRegistrar::registrar;


void AlgorithmRegistrar::validateLastRegistration() {
    const auto& last = algorithms.back();
    if (!last.hasPlayerFactory() || !last.hasTankAlgorithmFactory()) {
        std::stringstream msg;
        msg << last.name()
            << ": has player factory: " << last.hasPlayerFactory()
            << ", has tank algorithm factory: " << last.hasTankAlgorithmFactory();
        throw BadRegistrationException(msg.str());
    }
    LOG(LogLevel::INFO, std::string("Successfully validated registration for ") + last.name());
}

void AlgorithmRegistrar::duplicateFirstEntry() {
    LOG(LogLevel::INFO, std::string("Duplicating Player&Algo factories for ") + algorithms.front().name());
    if (algorithms.empty()) return;
    AlgorithmAndPlayerFactories copy(algorithms.front());
    algorithms.push_back(std::move(copy));
}