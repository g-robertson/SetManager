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
#include "intersection-set.hpp"
#include "union-set.hpp"
#include "difference-set.hpp"
#include "symmetric-difference-set.hpp"
#include "relative-complement-set.hpp"

#include "helpers.hpp"

#include <set>
#include <string>
#include <stack>
#include <iostream>
#include <fstream>
#include <stdexcept>

const std::set<pstring> UserSet::NO_ELEMENTS = std::set<pstring>();

const pstring UserSet::EXIT_KEYWORD = pliteral("EXIT");
const std::filesystem::path UserSet::DEFAULT_MACHINE_LOCATION = "managed-sets.txt";
const std::filesystem::path UserSet::DEFAULT_HUMAN_LOCATION = "human-readable-sets.txt";

UserSet* UserSet::EXIT_SET_MENU(UserSet&, const pstring&) noexcept {
    return nullptr;
}

UserSet::UserSet(
    std::unique_ptr<std::set<pstring>> elements,
    std::unique_ptr<std::set<pstring>> complementElements
) noexcept
    : elements_(std::move(elements)), complementElements_(std::move(complementElements))
{}

UserSet::UserSet(
    UserSet* parent,
    std::unique_ptr<std::set<pstring>> elements,
    std::unique_ptr<std::set<pstring>> complementElements
) noexcept
    : parent_(parent), elements_(std::move(elements)), complementElements_(std::move(complementElements))
{}

bool UserSet::preQuery() noexcept {
    return true;
}

void UserSet::postParentLoad() noexcept(false) {
    updateElements();
    for (const auto& subset : subsets_) {
        subset.second->postParentLoad();
    }
}

void UserSet::postSiblingsLoad() noexcept(false) {
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

const auto SUBSET_QUERY_MENU = StaticMenu<UserSet, UserSet*>({
    {pliteral("S"), {pliteral("Select a subset to use"), &UserSet::selectForSubset}},
    {pliteral("E"), {pliteral("Enter a subset and select from its subsets"), &UserSet::enterForSubset}},
    {UserSet::EXIT_KEYWORD, {pliteral("Exit subset menu"), &UserSet::exitSubsetMenu}}
});

UserSet* UserSet::queryForSubset() noexcept {
    return SUBSET_QUERY_MENU.query(*this);
}

UserSet* UserSet::selectForSubset() noexcept {
    pstring name;
    decltype(subsets_)::iterator subsetIt;
    
    pCinIgnoreAll();;
    do {
        if (!name.empty()) {
            pcout << "Enter a name that exists.\n";
        }

        listSubsets();
        pcout << "Enter the name of the list that you want to use for this option (case-sensitive) or \"" << UserSet::EXIT_KEYWORD << "\" to exit: ";
        pCinGetLine(name);
        if (insensitiveSame(name, UserSet::EXIT_KEYWORD)) {
            return nullptr;
        }
        subsetIt = subsets_.find(name);
    } while (subsetIt == subsets_.end());
    return &(*subsetIt->second);
}

UserSet* UserSet::enterForSubset() noexcept {
    auto* subset = selectForSubset();
    if (subset == nullptr) {
        return nullptr;
    }

    return subset->queryForSubset();
}

UserSet* UserSet::exitSubsetMenu() noexcept {
    return nullptr;
}

const auto USER_SET_MENU = StaticMenu<UserSet, void>({
    {pliteral("V"), {pliteral("View set-specific options"), &UserSet::setSpecificOptions}},
    {pliteral("H"), {pliteral("Create a human-readable output of the subsets"), &UserSet::saveHumanAllConnectedSubsets}},
    {pliteral("S"), {pliteral("Save all connected subsets"), &UserSet::saveMachineAllConnectedSubsets}},
    {pliteral("L"), {pliteral("Load all connected subsets"), &UserSet::loadMachineAllConnectedSubsets}},
    {pliteral("U"), {pliteral("Update internal elements"), &UserSet::updateInternalElements}},
    {pliteral("T"), {pliteral("Toggle whether or not this subset is inclued in human readable output"), &UserSet::toggleHumanInclusion}},
    {pliteral("TR"), {pliteral("Toggle whether or not this subset and all of its nested children are included in human readable output"), &UserSet::toggleHumanInclusionRecursively}},
    {pliteral("LS"), {pliteral("List subsets"), &UserSet::listSubsets}},
    {pliteral("LE"), {pliteral("List elements"), &UserSet::listElements}},
    {pliteral("C"), {pliteral("Create subset"), &UserSet::createSubset}},
    {pliteral("D"), {pliteral("Delete a subset"), &UserSet::deleteSubset}},
    {pliteral("E"), {pliteral("Enter a subset"), &UserSet::enterSubset}},
    {pliteral("X"), {pliteral("Move up one set hierarchy, or exit program if at top"), &UserSet::moveUpHierarchy}},
    {UserSet::EXIT_KEYWORD, {pliteral("Exit the program"), &UserSet::exitProgram}}
});

const Menu<UserSet, void>& UserSet::menu() const noexcept {
    return USER_SET_MENU;
}

const auto USER_SET_SPECIFIC_BASE_MENU = StaticMenu<UserSet, void>({
    {pliteral("X"), {pliteral("Exit set-specific options"), &UserSet::exitSetSpecificOptions}},
    {UserSet::EXIT_KEYWORD, {pliteral("Exit the program"), &UserSet::exitProgram}}
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

void UserSet::saveAllConnectedSubsets(void (UserSet::*saveMethod)(std::ostream& saveLocation), const std::filesystem::path& defaultSaveLocation) noexcept {
    std::filesystem::path saveLocation;
    pcout << "Enter a location to save to\n"
              << "\"d\" will output to default location (" << defaultSaveLocation << ", will be loaded automatically if the program is run in the same directory).\n"
              << "\"-\" will output to STDOUT.\n"
              << "or \"" << UserSet::EXIT_KEYWORD << "\" to exit: ";
    pCinIgnoreAll();;
    pCinGetLine(saveLocation);

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
        std::ofstream saveFileLocation(saveLocation.c_str());
        (globalSet->*saveMethod)(saveFileLocation);
    }
}

void UserSet::loadAllConnectedSubsets(void (UserSet::*loadMethod)(std::istream& loadLocation), const std::filesystem::path& defaultLoadLocation) noexcept {
    std::filesystem::path loadLocation;
    pcout << "Enter a location to load from\n"
              << "\"d\" will input from default location (" << defaultLoadLocation << ", will be loaded automatically if the program is run in the same directory).\n"
              << "\"-\" will input from STDOUT.\n"
              << "or \"" << UserSet::EXIT_KEYWORD << "\" to exit: ";
    pCinIgnoreAll();;
    pCinGetLine(loadLocation);

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
        std::ifstream loadFileLocation(loadLocation.c_str());
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
    if (!humanIncluded) {
        for (const auto& subset : subsets_) {
            subset.second->saveHumanSubsets_(saveLocation, indentation);
            onQuery();
        }
        return;
    }
    saveLocation << std::string(indentation, ' ') << genericStringFromPString(name()) << " {\n"
                 << std::string(indentation + 2, ' ');
    auto* elems = elements();
    if (elems == nullptr) {
        saveLocation << "Inverse ";
        elems = complementElements();
    }
    saveLocation << "Elements {";
    
    auto elemIt = elems->begin();
    if (elemIt != elems->end()) {
        saveLocation << '\n' << std::string(indentation + 6, ' ') << '\'' << genericStringFromPString(*elemIt) << '\'';
        ++elemIt;
    }
    for (; elemIt != elems->end(); ++elemIt) {
        saveLocation << ",\n" << std::string(indentation + 6, ' ') << '\'' << genericStringFromPString(*elemIt) << '\'';
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
    auto genericStringName = genericStringFromPString(name());
    saveLocation << type() << ' ' << humanIncluded << ' ' << genericStringName.size() << ' ' << genericStringName << ' ';
    saveMachineSubset(saveLocation);
    saveLocation << '\n';
    for (const auto& subset : subsets_) {
        subset.second->saveMachineSubsets(saveLocation);

        onQuery();
    }
    saveLocation << "0\n";
}

void UserSet::loadMachineSubsets_(std::istream& loadLocation) noexcept(false) {
    loadMachineSubset(loadLocation);
    while (true) {
        onQuery();

        char type;
        loadLocation >> type;
        if (type == '0') {
            break;
        }

        bool humanIncluded_;
        loadLocation >> humanIncluded_;

        int nameSize;
        loadLocation >> nameSize;

        skipRead(loadLocation, 1);

        auto genericStringName = std::string(nameSize, '0');
        loadLocation.read(genericStringName.data(), nameSize);
        pstring name = genericStringToPString(std::move(genericStringName));
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
            case IntersectionSet::type_:
                subsets_[name] = std::make_unique<IntersectionSet>(this, name);
                break;
            case UnionSet::type_:
                subsets_[name] = std::make_unique<UnionSet>(this, name);
                break;
            case DifferenceSet::type_:
                subsets_[name] = std::make_unique<DifferenceSet>(this, name);
                break;
            case SymmetricDifferenceSet::type_:
                subsets_[name] = std::make_unique<SymmetricDifferenceSet>(this, name);
                break;
            case RelativeComplementSet::type_:
                subsets_[name] = std::make_unique<RelativeComplementSet>(this, name);
                break;
            case GlobalSet::type_:
            default:
                throw std::logic_error("Global or non-defined type found in load case");
        }
        subsets_.at(name)->humanIncluded = humanIncluded_;
        subsets_.at(name)->loadMachineSubsets_(loadLocation);
    }
    for (const auto& subset : subsets_) {
        subset.second->postSiblingsLoad();
    }
}

void UserSet::loadMachineSubsets(std::istream& loadLocation) noexcept {
    char subtype;
    loadLocation >> subtype;

    loadLocation >> humanIncluded;

    int subsetNameSize;
    loadLocation >> subsetNameSize;

    skipRead(loadLocation, 1);

    auto genericStringSubsetName = std::string(subsetNameSize, '0');
    loadLocation.read(genericStringSubsetName.data(), subsetNameSize);
    pstring subsetName = genericStringToPString(std::move(genericStringSubsetName));
    bool caughtError = false;
    try {
        if (subtype != type()) {
            throw std::logic_error("type mismatch in head set");
        }
        if (subsetName != name()) {
            throw std::logic_error("name mismatch in head set");
        }

        subsets_.clear();
        loadMachineSubsets_(loadLocation);
        postParentLoad();
    } catch (const std::logic_error& error) {
        pcout << "[IMPORTANT ERROR]\n"
                  << "[IMPORTANT ERROR]\n"
                  << "[IMPORTANT ERROR]\n"
                  << "Failed to load subsets due to '" << error.what() << "'.\n"
                  << "[IMPORTANT ERROR]\n"
                  << "[IMPORTANT ERROR]\n"
                  << "[IMPORTANT ERROR]\n";
        caughtError = true;
    } catch (...) {
        pcout << "[FATAL ERROR]\n"
                  << "[FATAL ERROR]\n"
                  << "[FATAL ERROR]\n"
                  << "I DO NOT KNOW WHAT HAPPENED TO CAUSE THIS, PLEASE REPORT THIS IF YOU FIND IT\n"
                  << "[FATAL ERROR]\n"
                  << "[FATAL ERROR]\n"
                  << "[FATAL ERROR]\n";
        caughtError = true;
    }
    if (caughtError) {
        loadLocation.seekg(0);
        if (loadLocation.tellg() == 0) {
            std::filesystem::path backupLocation = DEFAULT_MACHINE_LOCATION;
            backupLocation.append(".bak");
            std::ofstream backupFile(backupLocation.c_str());
            std::ostringstream osstr;
            loadLocation >> osstr.rdbuf();
            std::string remainingContents = osstr.str();

            backupFile.write(remainingContents.data(), remainingContents.size());

            pcout << "Backed up loaded data to '" << backupLocation << "'\n";
        }
        subsets_.clear();
    }
}

void UserSet::updateInternalElements() noexcept {
    if (parent_ != nullptr) {
        parent_->updateInternalElements();
    }
    updateElements();
}

void UserSet::toggleHumanInclusion() noexcept {
    humanIncluded = !humanIncluded;
    pcout << "Human inclusion of this subset was turned " << (humanIncluded ? "on" : "off") << ".\n";
}

void UserSet::toggleHumanInclusionRecursively_(bool state) noexcept {
    humanIncluded = state;
    for (const auto& subset : subsets_) {
        subset.second->toggleHumanInclusionRecursively_(state);
    }
}

void UserSet::toggleHumanInclusionRecursively() noexcept {
    humanIncluded = !humanIncluded;
    toggleHumanInclusionRecursively_(humanIncluded);
    pcout << "Human inclusion of this subset and all of its nested children was turned " << (humanIncluded ? "on" : "off") << ".\n";
}

void UserSet::listSubsets() noexcept {
    pcout << "Subset list\n";
    pcout << pstring(80, '-') << '\n';
    for (const auto& subsetKVP : subsets_) {
        pcout << subsetKVP.first << '\n';
    }
    pcout << pstring(80, '-') << '\n';
}

void UserSet::listElements() noexcept {
    pcout << "Element list\n";
    pcout << pstring(80, '-') << '\n';
    if (elements() == nullptr) {
        pcout << "This set contains every element except for:\n";
        for (const auto& element : *complementElements()) {
            pcout << '\'' << element << "'\n";
        }
    } else {
        pcout << "This set contains:\n";
        for (const auto& element : *elements()) {
            pcout << '\'' << element << "'\n";
        }
    }
    pcout << pstring(80, '-') << '\n';
}

void UserSet::createSubset() noexcept {
    pstring name;

    pCinIgnoreAll();;
    do {
        if (!name.empty()) {
            pcout << "Enter a name not previously used.\n";
        }
        pcout << "Enter a name for the subset or \"" << UserSet::EXIT_KEYWORD << "\" to exit: ";
        pCinGetLine(name);
        if (insensitiveSame(name, UserSet::EXIT_KEYWORD)) {
            return;
        }
    } while (subsets_.find(name) != subsets_.end());

    pcout << "Enter a type for the subset\n";

    auto* subset = createableSubsetMenu().query(*this, name);
    if (subset == nullptr) {
        return;
    }
    subsets_[name] = std::unique_ptr<UserSet>(subset);
}

void UserSet::deleteSubset() noexcept {
    auto* subset = selectForSubset();
    if (subset == nullptr) {
        return;
    }

    subsets_.erase(pstring(subset->name()));
}

void UserSet::enterSubset() noexcept {
    auto* subset = selectForSubset();
    if (subset == nullptr) {
        return;
    }

    while (subset->query());
}

void UserSet::moveUpHierarchy() noexcept {
    queryable = false;
}

void UserSet::exitProgram() noexcept {
    pcout << "Do you want to save to default location before exiting? ('n' for no, anything else assumed yes): ";
    pCinIgnoreAll();;
    pchar c;
    pCinGet(c);
    if (std::tolower(c) != 'n') {
        std::ofstream defaultLocation(UserSet::DEFAULT_MACHINE_LOCATION.c_str());
        UserSet* global = this;
        while (global->parent() != nullptr) {
            global = global->parent();
        }
        global->saveMachineSubsets(defaultLocation);
    }
    exit(0);
}

const std::set<pstring>* UserSet::elements() const noexcept {
    return elements_.get();
}

const std::set<pstring>* UserSet::complementElements() const noexcept {
    return complementElements_.get();
}

bool UserSet::contains(const pstring& element) const noexcept {
    if (elements_.get() != nullptr) {
        return elements_->count(element) == 1;
    } else {
        return complementElements_->count(element) == 0;
    }
}

void UserSet::removedElement(const pstring& element, bool expected) noexcept {
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

const std::map<pstring, std::unique_ptr<UserSet>>& UserSet::subsets() const noexcept {
    return subsets_;
}

void UserSet::onQuery() noexcept {
    if (onQueryRemove != nullptr) {
        subsets_.erase(pstring(onQueryRemove->name()));
        onQueryRemove = nullptr;
    }
    if (onQueryAdd) {
        subsets_.insert({pstring(onQueryAdd->name()), std::move(onQueryAdd)});
    }
}