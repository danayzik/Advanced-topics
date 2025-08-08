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
}

void AlgorithmRegistrar::duplicateFirstEntry() {
    if (algorithms.empty()) return;
    AlgorithmAndPlayerFactories copy(algorithms.front());
    algorithms.push_back(std::move(copy));
}