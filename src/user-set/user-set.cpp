/*
    user-set.cpp

    UserSet is the most abstract type of set, containing all methods that are possible for all set types to have
    It serves as the interface that all sets can interact with eachother with
*/
#include "user-set.hpp"

#include "global-set.hpp"
#include "word-set.hpp"
#include "faux-word-set.hpp"
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

UserSet* UserSet::EXIT_SET_MENU(UserSet&, const std::string&) noexcept {
    return nullptr;
}

UserSet::UserSet(UserSet* userSet) noexcept
    : parent_(userSet)
{}

bool UserSet::preQuery() noexcept {
    return true;
}

bool UserSet::postParentLoad() noexcept {
    return false;
}

bool UserSet::query() noexcept {
    queryable = true;
    onQuery();

    if (onQueryEnter != nullptr) {
        while (onQueryEnter->query());
        onQueryEnter = nullptr;
        return queryable;
    }
    if (preQuery()) {
        menu().query(*this);
    }
    return queryable;
}

const auto USER_SET_MENU = StaticMenu<UserSet, void>({
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

const Menu<UserSet, void>& UserSet::menu() const noexcept {
    return USER_SET_MENU;
}

const auto USER_SET_SPECIFIC_BASE_MENU = StaticMenu<UserSet, void>({
    {"X", {"Exit set-specific options", &UserSet::exitSetSpecificOptions}},
    {UserSet::EXIT_KEYWORD, {"Exit the program", &UserSet::exitProgram}}
});

const Menu<UserSet, void>& UserSet::setSpecificMenu() const noexcept {
    return USER_SET_SPECIFIC_BASE_MENU;
}

void UserSet::setSpecificOptions() noexcept {
    setSpecificQueryable = true;
    while (setSpecificQueryable) {
        setSpecificMenu().query(*this);
    }
}

void UserSet::exitSetSpecificOptions() noexcept {
    setSpecificQueryable = false;
}

void UserSet::saveAllConnectedSubsets(void (UserSet::*saveMethod)(std::ostream& saveLocation), std::string_view defaultSaveLocation) noexcept {
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
    
    UserSet* globalSet = this;
    while (globalSet->parent() != nullptr) {
        globalSet = globalSet->parent();
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

void UserSet::loadAllConnectedSubsets(void (UserSet::*loadMethod)(std::istream& loadLocation), std::string_view defaultLoadLocation) noexcept {
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
    while (globalSet->parent() != nullptr) {
        globalSet = globalSet->parent();
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

void UserSet::saveHumanAllConnectedSubsets() noexcept {
    saveAllConnectedSubsets(&UserSet::saveHumanSubsets, DEFAULT_HUMAN_LOCATION);
}

void UserSet::saveHumanSubsets(std::ostream& saveLocation) noexcept {
    saveHumanSubsets_(saveLocation, 0);
}

void UserSet::saveHumanSubsets_(std::ostream& saveLocation, int indentation) noexcept {
    onQuery();

    saveLocation << std::string(indentation, ' ') << name() << " {\n"
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
    for (const auto& subset : subsets_) {
        subset.second->saveHumanSubsets_(saveLocation, indentation + 4);
        onQuery();
    }
    saveLocation << std::string(indentation + 2, ' ') << "}\n"
                 << std::string(indentation, ' ') << "}\n";
}

void UserSet::saveMachineAllConnectedSubsets() noexcept {
    saveAllConnectedSubsets(&UserSet::saveMachineSubsets, DEFAULT_MACHINE_LOCATION);
}

void UserSet::loadMachineAllConnectedSubsets() noexcept {
    loadAllConnectedSubsets(&UserSet::loadMachineSubsets, DEFAULT_MACHINE_LOCATION);
}

void UserSet::saveMachineSubsets(std::ostream& saveLocation) noexcept {
    onQuery();

    saveLocation << type() << ' ' << name().size() << ' ' << name() << ' ';
    saveMachineSubset(saveLocation);
    saveLocation << '\n';
    for (const auto& subset : subsets_) {
        subset.second->saveMachineSubsets(saveLocation);

        onQuery();
    }
    saveLocation << "0\n";
}

bool UserSet::loadMachineSubsets_(std::istream& loadLocation) noexcept {
    loadMachineSubset(loadLocation);
    while (!loadFailed) {
        onQuery();

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
            case WordSet::type_:
                subsets_[name] = std::make_unique<WordSet>(this, name);
                break;
            case FauxWordSet::type_:
                subsets_[name] = std::make_unique<FauxWordSet>(this, name);
                break;
            case DirectorySet::type_:
                subsets_[name] = std::make_unique<DirectorySet>(this, name);
                break;
            case GlobalSet::type_:
            default:
                return true;
        }
        loadFailed = subsets_.at(name)->loadMachineSubsets_(loadLocation);
    }
    for (const auto& subset : subsets_) {
        if (subset.second->postParentLoad()) {
            return true;
        }
    }
    return false;
}

void UserSet::loadMachineSubsets(std::istream& loadLocation) noexcept {
    char subtype;
    loadLocation >> subtype;

    int subsetNameSize;
    loadLocation >> subsetNameSize;

    skipRead(loadLocation, 1);

    auto subsetName = std::string(subsetNameSize, '0');
    loadLocation.read(subsetName.data(), subsetNameSize);
    if (subtype != type() || subsetName != name()) {
        loadFailed = true;
    }
    if (!loadFailed) {
        subsets_.clear();
        loadMachineSubsets_(loadLocation);
    }
    if (loadFailed) {
        std::cout << "[IMPORTANT ERROR]\n"
                  << "[IMPORTANT ERROR]\n"
                  << "[IMPORTANT ERROR]\n"
                  << "Failed to load subsets due to improper subset file format.\n"
                  << "[IMPORTANT ERROR]\n"
                  << "[IMPORTANT ERROR]\n"
                  << "[IMPORTANT ERROR]\n";
        loadLocation.seekg(0);
        if (loadLocation.tellg() == 0) {
            std::string backupLocation = DEFAULT_MACHINE_LOCATION + ".bak";
            std::ofstream backupFile(backupLocation);
            std::ostringstream osstr;
            loadLocation >> osstr.rdbuf();
            std::string remainingContents = osstr.str();

            backupFile.write(remainingContents.data(), remainingContents.size());

            std::cout << "Backed up loaded data to '" << backupLocation << "'\n";
        }
        subsets_.clear();
        loadFailed = false;
    }
}

void UserSet::listSubsets() noexcept {
    std::cout << "Subset list\n";
    std::cout << std::string(80, '-') << '\n';
    for (const auto& subsetKVP : subsets_) {
        std::cout << subsetKVP.first << '\n';
    }
    std::cout << std::string(80, '-') << '\n';
}

void UserSet::listElements() noexcept {
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
}

void UserSet::createSubset() noexcept {
    std::string name;

    ignoreAll(std::cin);
    do {
        if (!name.empty()) {
            std::cout << "Enter a name not previously used.\n";
        }
        std::cout << "Enter a name for the subset or \"" << UserSet::EXIT_KEYWORD << "\" to exit: ";
        std::getline(std::cin, name);
        if (insensitiveSame(name, UserSet::EXIT_KEYWORD)) {
            return;
        }
    } while (subsets_.find(name) != subsets_.end());

    std::cout << "Enter a type for the subset\n";

    auto* subset = createableSubsetMenu().query(*this, name);
    if (subset == nullptr) {
        return;
    }
    subsets_[name] = std::unique_ptr<UserSet>(subset);
}

void UserSet::deleteSubset() noexcept {
    std::string name;
    decltype(subsets_)::iterator subsetIt;

    ignoreAll(std::cin);
    do {
        if (!name.empty()) {
            std::cout << "Enter a name that exists.\n";
        }

        listSubsets();
        std::cout << "Enter the name of the list that you want to enter (case-sensitive) or \"" << UserSet::EXIT_KEYWORD << "\" to exit: ";
        std::getline(std::cin, name);
        if (insensitiveSame(name, UserSet::EXIT_KEYWORD)) {
            return;
        }
        subsetIt = subsets_.find(name);
    } while (subsetIt == subsets_.end());

    subsets_.erase(subsetIt);
}

void UserSet::enterSubset() noexcept {
    std::string name;
    decltype(subsets_)::iterator subsetIt;
    
    ignoreAll(std::cin);
    do {
        if (!name.empty()) {
            std::cout << "Enter a name that exists.\n";
        }

        listSubsets();
        std::cout << "Enter the name of the list that you want to enter (case-sensitive) or \"" << UserSet::EXIT_KEYWORD << "\" to exit: ";
        std::getline(std::cin, name);
        if (insensitiveSame(name, UserSet::EXIT_KEYWORD)) {
            return;
        }
        subsetIt = subsets_.find(name);
    } while (subsetIt == subsets_.end());

    while (subsetIt->second->query());
}

void UserSet::moveUpHierarchy() noexcept {
    queryable = false;
}

void UserSet::exitProgram() noexcept {
    std::cout << "Do you want to save to default location before exiting? ('n' for no, anything else assumed yes): ";
    ignoreAll(std::cin);
    char c;
    std::cin.get(c);
    if (std::tolower(c) != 'n') {
        std::ofstream defaultLocation(UserSet::DEFAULT_MACHINE_LOCATION);
        UserSet* global = this;
        while (global->parent() != nullptr) {
            global = global->parent();
        }
        global->saveMachineSubsets(defaultLocation);
    }
    exit(0);
}

bool UserSet::contains(const std::string& element) const noexcept {
    if (elements() != nullptr) {
        return elements()->count(element) == 1;
    } else {
        return complementElements()->count(element) == 0;
    }
}

void UserSet::removedElement(const std::string& element, bool expected) noexcept {
    for (const auto& subset : subsets_) {
        subset.second->removedElement(element, expected);
    }
}

const UserSet* UserSet::parent() const noexcept {
    return parent_;
}

UserSet* UserSet::parent() noexcept {
    return parent_;
}

const std::map<std::string, std::unique_ptr<UserSet>>& UserSet::subsets() const noexcept {
    return subsets_;
}

void UserSet::onQuery() noexcept {
    if (onQueryRemove != nullptr) {
        subsets_.erase(std::string(onQueryRemove->name()));
        onQueryRemove = nullptr;
    }
    if (onQueryAdd) {
        subsets_.insert({std::string(onQueryAdd->name()), std::move(onQueryAdd)});
    }
}