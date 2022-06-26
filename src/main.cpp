#include "global-set.hpp"

#include <filesystem>
#include <fstream>

GlobalSet GLOBAL_SET;

int main() {
    // load from default machine location if it exists
    if (std::filesystem::exists(UserSet::DEFAULT_MACHINE_LOCATION)) {
        std::ifstream defaultMachineLocation(UserSet::DEFAULT_MACHINE_LOCATION);
        GLOBAL_SET.loadMachineSubsets(defaultMachineLocation);
    }
    while (GLOBAL_SET.query());
    // exit with the intended exit dialogue
    GLOBAL_SET.exitProgram();
}