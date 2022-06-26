#include "user-set.hpp"

#include "global-set.hpp"
#include "word-set.hpp"
#include "directory-set.hpp"
#include "helpers.hpp"

#include <set>
#include <string>
#include <stack>
#include <iostream>
#include <fstream>
#include <stdexcept>

const std::set<std::string> UserSet::NO_ELEMENTS = std::set<std::string>();

const std::string UserSet::EXIT_KEYWORD = "EXIT";
const std::string UserSet::DEFAULT_MACHINE_LOCATION = "managed-sets.txt";
const std::string UserSet::DEFAULT_HUMAN_LOCATION = "human-readable-sets.txt";

std::unique_ptr<UserSet> UserSet::EXIT_SET_MENU(UserSet&, const std::string&) {
    return std::unique_ptr<UserSet>();
}

UserSet::UserSet(UserSet* userSet)
    : parent(userSet)
{}

bool UserSet::query() {
    return menu().query(*this);
}

const auto USER_SET_MENU = StaticMenu<UserSet, bool>({
    {"V", {"View set-specific options", &UserSet::setSpecificOptions}},
    {"H", {"Create a human-readable output of the subsets", &UserSet::saveHumanAllConnectedSubsets}},
    {"S", {"Save all connected subsets", &UserSet::saveMachineAllConnectedSubsets}},
    {"L", {"Load all connected subsets", &UserSet::loadMachineAllConnectedSubsets}},
    {"LS", {"List subsets", &UserSet::listSubsets}},
    {"LE", {"List elements", &UserSet::listElements}},
    {"C", {"Create subset", &UserSet::createSubset}},
    {"D", {"Delete a subset", &UserSet::deleteSubset}},
    {"E", {"Enter a subset", &UserSet::enterSubset}},
    {"X", {"Move up one set hierarchy, or exit program if at top", &UserSet::moveUpHierarchy}},
    {UserSet::EXIT_KEYWORD, {"Exit the program", &UserSet::exitProgram}}
});

const Menu<UserSet, bool>& UserSet::menu() const {
    return USER_SET_MENU;
}

const auto USER_SET_SPECIFIC_BASE_MENU = StaticMenu<UserSet, bool>({
    {"X", {"Exit set-specific options", &UserSet::moveUpHierarchy}},
    {UserSet::EXIT_KEYWORD, {"Exit the program", &UserSet::exitProgram}}
});

const Menu<UserSet, bool>& UserSet::setSpecificMenu() const {
    return USER_SET_SPECIFIC_BASE_MENU;
}

bool UserSet::setSpecificOptions() {
    while (setSpecificMenu().query(*this));
    return true;
}

void UserSet::saveAllConnectedSubsets(void (UserSet::*saveMethod)(std::ostream& saveLocation) const, std::string_view defaultSaveLocation) const {
    std::string saveLocation;
    std::cout << "Enter a location to save to\n"
              << "\"d\" will output to default location (\"" << defaultSaveLocation << "\", will be loaded automatically if the program is run in the same directory).\n"
              << "\"-\" will output to STDOUT.\n"
              << "or \"" << UserSet::EXIT_KEYWORD << "\" to exit: ";
    ignoreAll(std::cin);
    std::getline(std::cin, saveLocation);

    if (insensitiveSame(saveLocation, UserSet::EXIT_KEYWORD)) {
        return;
    }
    
    const UserSet* globalSet = this;
    while (globalSet->parent != nullptr) {
        globalSet = globalSet-> parent;
    }
    if (saveLocation == "d") {
        saveLocation = defaultSaveLocation;
    }
    if (saveLocation == "-") {
        (globalSet->*saveMethod)(std::cout);
    } else {
        std::ofstream saveFileLocation(saveLocation);
        (globalSet->*saveMethod)(saveFileLocation);
    }
}

void UserSet::loadAllConnectedSubsets(void (UserSet::*loadMethod)(std::istream& loadLocation), std::string_view defaultLoadLocation) {
    std::string loadLocation;
    std::cout << "Enter a location to load from\n"
              << "\"d\" will input from default location (\"" << defaultLoadLocation << "\", will be loaded automatically if the program is run in the same directory).\n"
              << "\"-\" will input from STDOUT.\n"
              << "or \"" << UserSet::EXIT_KEYWORD << "\" to exit: ";
    ignoreAll(std::cin);
    std::getline(std::cin, loadLocation);

    if (insensitiveSame(loadLocation, UserSet::EXIT_KEYWORD)) {
        return;
    }

    UserSet* globalSet = this;
    while (globalSet->parent != nullptr) {
        globalSet = globalSet->parent;
    }
    if (loadLocation == "d") {
        loadLocation = defaultLoadLocation;
    }
    if (loadLocation == "-") {
        (globalSet->*loadMethod)(std::cin);
    } else {
        std::ifstream loadFileLocation(loadLocation);
        (globalSet->*loadMethod)(loadFileLocation);
    }
}

bool UserSet::saveHumanAllConnectedSubsets() {
    saveAllConnectedSubsets(&UserSet::saveHumanSubsets, DEFAULT_HUMAN_LOCATION);
    return true;
}

void UserSet::saveHumanSubsets(std::ostream& saveLocation) const {
    saveHumanSubsets_(saveLocation, 0);
}

void UserSet::saveHumanSubsets_(std::ostream& saveLocation, int indentation) const {
    saveLocation << std::string(indentation, ' ') << setName() << " {\n"
                 << std::string(indentation + 2, ' ');
    auto* elems = elements();
    if (elems == nullptr) {
        saveLocation << "Inverse ";
        elems = complementElements();
    }
    saveLocation << "Elements {";
    
    auto elemIt = elems->begin();
    if (elemIt != elems->end()) {
        saveLocation << '\n' << std::string(indentation + 6, ' ') << '\'' << *elemIt << '\'';
        ++elemIt;
    }
    for (; elemIt != elems->end(); ++elemIt) {
        saveLocation << ",\n" << std::string(indentation + 6, ' ') << '\'' << *elemIt << '\'';
    }
    saveLocation << '\n' << std::string(indentation + 2, ' ') << "},\n"
                 << std::string(indentation + 2, ' ') << "Subsets {\n";
    for (const auto& subset : subsets) {
        subset.second->saveHumanSubsets_(saveLocation, indentation + 4);
    }
    saveLocation << std::string(indentation + 2, ' ') << "}\n"
                 << std::string(indentation, ' ') << "}\n";
}

bool UserSet::saveMachineAllConnectedSubsets() {
    saveAllConnectedSubsets(&UserSet::saveMachineSubsets, DEFAULT_MACHINE_LOCATION);
    return true;
}

bool UserSet::loadMachineAllConnectedSubsets() {
    loadAllConnectedSubsets(&UserSet::loadMachineSubsets, DEFAULT_MACHINE_LOCATION);
    return true;
}

void UserSet::saveMachineSubsets(std::ostream& saveLocation) const {
    saveLocation << setType() << ' ' << setName().size() << ' ' << setName() << ' ';
    saveMachineSubset(saveLocation);
    saveLocation << '\n';
    for (const auto& subset : subsets) {
        subset.second->saveMachineSubsets(saveLocation);
    }
    saveLocation << "0\n";
}

void UserSet::loadMachineSubsets_(std::istream& loadLocation) {
    loadMachineSubset(loadLocation);
    
    while (true) {
        char type;
        loadLocation >> type;
        if (type == '0') {
            break;
        }

        int nameSize;
        loadLocation >> nameSize;

        skipRead(loadLocation, 1);

        auto name = std::string(nameSize, '0');
        loadLocation.read(name.data(), nameSize);

        switch (type) {
            case WordSet::setType_():
                subsets[name] = std::make_unique<WordSet>(this, name);
                break;
            case DirectorySet::setType_():
                subsets[name] = std::make_unique<DirectorySet>(this, name);
                break;
            case GlobalSet::setType_():
            default:
                std::cout << "Failed to load subsets due to improper subset file format (global or unknown type in tree).\n";
                throw std::logic_error("Partial load, resolveable unaccounted for. [Partial load due to global or unknown type]");
                break;
        }
        subsets.at(name)->loadMachineSubsets_(loadLocation);
    }

}

void UserSet::loadMachineSubsets(std::istream& loadLocation) {
    char type;
    loadLocation >> type;

    int nameSize;
    loadLocation >> nameSize;

    skipRead(loadLocation, 1);

    auto name = std::string(nameSize, '0');
    loadLocation.read(name.data(), nameSize);
    if (type != setType() || name != setName()) {
        std::string backupLocation = DEFAULT_MACHINE_LOCATION + ".bak";
        std::ofstream backupFile(backupLocation);
        backupFile << type << ' ' << nameSize << ' ' << name << ' ';
        
        std::ostringstream osstr;
        loadLocation >> osstr.rdbuf();
        std::string remainingContents = osstr.str();
        
        backupFile.write(remainingContents.data(), remainingContents.size());

        std::cout << "[IMPORTANT ERROR]\n"
                  << "[IMPORTANT ERROR]\n"
                  << "[IMPORTANT ERROR]\n"
                  << "Failed to load subsets due to improper subset file format (mismatch type or name).\n"
                  << "Backed up loaded data to '" << backupLocation << "'\n"
                  << "[IMPORTANT ERROR]\n"
                  << "[IMPORTANT ERROR]\n"
                  << "[IMPORTANT ERROR]\n";
        return;
    }
    subsets.clear();
    loadMachineSubsets_(loadLocation);
}

bool UserSet::listSubsets() {
    std::cout << "Subset list\n";
    std::cout << std::string(80, '-') << '\n';
    for (const auto& subsetKVP : subsets) {
        std::cout << subsetKVP.first << '\n';
    }
    std::cout << std::string(80, '-') << '\n';
    return true;
}

bool UserSet::listElements() {
    std::cout << "Element list\n";
    std::cout << std::string(80, '-') << '\n';
    if (elements() == nullptr) {
        std::cout << "This set contains every element except for:\n";
        for (const auto& element : *complementElements()) {
            std::cout << '\'' << element << "'\n";
        }
    } else {
        std::cout << "This set contains:\n";
        for (const auto& element : *elements()) {
            std::cout << '\'' << element << "'\n";
        }
    }
    std::cout << std::string(80, '-') << '\n';
    return true;
}

bool UserSet::createSubset() {
    std::string name;

    ignoreAll(std::cin);
    do {
        if (!name.empty()) {
            std::cout << "Enter a name not previously used.\n";
        }
        std::cout << "Enter a name for the subset or \"" << UserSet::EXIT_KEYWORD << "\" to exit: ";
        std::getline(std::cin, name);
        if (insensitiveSame(name, UserSet::EXIT_KEYWORD)) {
            return true;
        }
    } while (subsets.find(name) != subsets.end());

    std::cout << "Enter a type for the subset\n";

    auto subset = createableSubsetMenu().query(*this, name);
    if (!subset) {
        return true;
    }
    subsets[name] = std::move(subset);
    return true;
}

bool UserSet::deleteSubset() {
    std::string name;
    decltype(subsets)::iterator subsetIt;

    ignoreAll(std::cin);
    do {
        if (!name.empty()) {
            std::cout << "Enter a name that exists.\n";
        }

        listSubsets();
        std::cout << "Enter the name of the list that you want to enter (case-sensitive) or \"" << UserSet::EXIT_KEYWORD << "\" to exit: ";
        std::getline(std::cin, name);
        if (insensitiveSame(name, UserSet::EXIT_KEYWORD)) {
            return true;
        }
        subsetIt = subsets.find(name);
    } while (subsetIt == subsets.end());

    subsets.erase(subsetIt);
    return true;
}

bool UserSet::enterSubset() {
    std::string name;
    decltype(subsets)::iterator subsetIt;
    
    ignoreAll(std::cin);
    do {
        if (!name.empty()) {
            std::cout << "Enter a name that exists.\n";
        }

        listSubsets();
        std::cout << "Enter the name of the list that you want to enter (case-sensitive) or \"" << UserSet::EXIT_KEYWORD << "\" to exit: ";
        std::getline(std::cin, name);
        if (insensitiveSame(name, UserSet::EXIT_KEYWORD)) {
            return true;
        }
        subsetIt = subsets.find(name);
    } while (subsetIt == subsets.end());

    while (subsetIt->second->query());
    return true;
}

bool UserSet::moveUpHierarchy() {
    return false;
}

bool UserSet::exitProgram() {
    std::cout << "Do you want to save to default location before exiting? ('n' for no, anything else assumed yes): ";
    ignoreAll(std::cin);
    char c;
    std::cin.get(c);
    if (std::tolower(c) != 'n') {
        std::ofstream defaultLocation(UserSet::DEFAULT_MACHINE_LOCATION);
        const UserSet* global = this;
        while (global->parent != nullptr) {
            global = global->parent;
        }
        global->saveMachineSubsets(defaultLocation);
    }
    exit(0);
}

bool UserSet::contains(const std::string& element) const {
    if (elements() != nullptr) {
        return elements()->count(element) == 1;
    } else {
        return complementElements()->count(element) == 0;
    }
}

void UserSet::removedElement(const std::string& element) {
    for (const auto& subset : subsets) {
        subset.second->removedElement(element);
    }
}