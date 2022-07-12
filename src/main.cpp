#include "global-set.hpp"

#include "platform.hpp"

#include <filesystem>
#include <fstream>

GlobalSet GLOBAL_SET;

int main() {
    platform();
    // load from default machine location if it exists
    if (std::filesystem::exists(UserSet::DEFAULT_MACHINE_LOCATION)) {
        std::ifstream defaultMachineLocation(UserSet::DEFAULT_MACHINE_LOCATION.c_str());
        GLOBAL_SET.loadMachineSubsets(defaultMachineLocation);
    }
    while (GLOBAL_SET.query());
    // exit with the intended exit dialogue
    GLOBAL_SET.exitProgram();
}