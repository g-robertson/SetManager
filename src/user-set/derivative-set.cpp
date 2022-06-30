/*
    derivative-set.cpp

    DerivativeSet is a set that holds the properties of a subset, while also requiring elements of certain subsets of its parent set
    It is an abstract type of set due to not having any defined methods for gathering elements, complement elements, or whom it derives from
    Its purpose is to provide an interface for ensuring that all subsets necessary for a derivative subset are available before finishing its load

    Note: It is undefined behavior to instantiate this class with the parent, name constructor and then not run postParentLoad() hook
*/
#include "derivative-set.hpp"

DerivativeSet::DerivativeSet(UserSet* parent, const std::string& name, std::initializer_list<UserSet*> userSets) noexcept
    : SubSet(parent, name), derivesFrom__(new std::vector<UserSet*>(userSets))
{}

// Note: It is undefined behavior to instantiate this class with the parent, name constructor and then not run postParentLoad() hook
DerivativeSet::DerivativeSet(UserSet* parent, const std::string& name) noexcept
    : SubSet(parent, name), derivesFromNames__(new std::vector<std::string>)
{}

DerivativeSet::~DerivativeSet() noexcept {
    delete derivesFrom__;
}

const std::vector<UserSet*>& DerivativeSet::derivesFrom() const noexcept {
    return *derivesFrom__;
}

std::vector<UserSet*>& DerivativeSet::derivesFrom_() noexcept {
    return *derivesFrom__;
}

void DerivativeSet::saveMachineSubset(std::ostream& saveLocation) noexcept {
    saveLocation << derivesFrom__->size();
    for (const auto* userSet : *derivesFrom__) {
        saveLocation << ' ' << userSet->name().size() << userSet->name();
    }
    saveLocation << ' ';
    saveMachineDerivativeSubset(saveLocation);
}

void DerivativeSet::loadMachineSubset(std::istream& loadLocation) noexcept {
    int userSetsCount;
    loadLocation >> userSetsCount;
    derivesFromNames__->reserve(userSetsCount);
    for (; userSetsCount > 0; --userSetsCount) {
        int userSetSize;
        loadLocation >> userSetSize;
        std::string userSetName;
        userSetName.reserve(userSetSize);

        loadLocation >> userSetName;
        derivesFromNames__->push_back(std::move(userSetName));
    }
    loadMachineDerivativeSubset(loadLocation);
}

void DerivativeSet::saveMachineDerivativeSubset(std::ostream&) noexcept {
}

void DerivativeSet::loadMachineDerivativeSubset(std::istream&) noexcept {
}

bool DerivativeSet::postParentLoad() noexcept {
    if (postParentLoaded) {
        return false;
    }
    if (postParentLoading) {
        std::cout << "Recursive parent loading detected\n";
        return true;
    }
    postParentLoading = true;

    // sets up vector to store in derivesFrom__
    auto* userSets = new std::vector<UserSet*>;
    auto& parentSubsets = parent->subsets();
    bool failed = false;
    // adds all the names in derivesFromNames__ to userSets
    for (auto& userSetName : *derivesFromNames__) {
        auto userSetIt = parentSubsets.find(userSetName);
        if (userSetIt == parentSubsets.end()) {
            failed = true;
            break;
        }

        auto* userSet = userSetIt->second.get();
        if (userSet->postParentLoad()) {
            failed = true;
            break;
        }
        userSets->push_back(userSet);
    }
    // removes derivesFromNames__ and replaces it with derivesFrom__
    delete derivesFromNames__;
    derivesFrom__ = userSets;
    // only after the class is well formed by derivesFrom__ being defined do we allow it to finish short-circuit failing
    if (failed) {
        return true;
    }

    postParentLoading = false;
    postParentLoaded = true;
    return postPostParentLoad();
}

bool DerivativeSet::postPostParentLoad() noexcept {
    return false;
}