#include "global-set.hpp"

#include "platform.hpp"

#include <nowide/fstream.hpp>

GlobalSet GLOBAL_SET;

int main() {
    // load from default machine location if it exists
    if (std::filesystem::exists(UserSet::DEFAULT_MACHINE_LOCATION)) {
        nowide::ifstream defaultMachineLocation(denativePath(UserSet::DEFAULT_MACHINE_LOCATION));
        GLOBAL_SET.loadMachineSubsets(defaultMachineLocation);
    }
    while (GLOBAL_SET.query());
    // exit with the intended exit dialogue
    GLOBAL_SET.exitProgram();
}