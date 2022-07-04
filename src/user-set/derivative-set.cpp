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
    : SubSet(parent, name), derivesFromNames__(new std::vector<std::vector<std::string>>)
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
        std::stringstream nestedSubsetsWrite;
        int nestedCount = -1;
        for (; userSet != parent(); userSet = userSet->parent()) {
            nestedSubsetsWrite << ' ' << userSet->name().size() << userSet->name();
            ++nestedCount;
        }
        saveLocation << ' ' << nestedCount << nestedSubsetsWrite.str();
    }
    saveLocation << ' ';
    saveMachineDerivativeSubset(saveLocation);
}

void DerivativeSet::loadMachineSubset(std::istream& loadLocation) noexcept {
    int userSetsCount;
    loadLocation >> userSetsCount;
    derivesFromNames__->reserve(userSetsCount);
    for (; userSetsCount > 0; --userSetsCount) {
        int nestedCount;
        loadLocation >> nestedCount;
        std::vector<std::string> userSetNames;
        userSetNames.resize(nestedCount + 1);
        for (; nestedCount >= 0; --nestedCount) {
            int userSetSize;
            loadLocation >> userSetSize;
            std::string userSetName;
            userSetName.resize(userSetSize);
            loadLocation >> userSetName;

            userSetNames.insert(userSetNames.begin() + nestedCount, std::move(userSetName));
        }
        derivesFromNames__->push_back(std::move(userSetNames));
    }
    loadMachineDerivativeSubset(loadLocation);
}

void DerivativeSet::saveMachineDerivativeSubset(std::ostream&) noexcept {
}

void DerivativeSet::loadMachineDerivativeSubset(std::istream&) noexcept {
}

bool DerivativeSet::postParentLoad() noexcept {
    // if already loaded, then don't load again
    if (postParentLoaded) {
        return false;
    }
    // if in process of loading, then there is a recursive set somewhere, which is not allowed
    if (postParentLoading) {
        std::cout << "Recursive parent loading detected\n";
        return true;
    }
    postParentLoading = true;

    // sets up vector to store in derivesFrom__
    auto* userSets = new std::vector<UserSet*>;
    bool failed = false;
    // adds all the nested names in derivesFromNames__ to userSets
    for (const auto& userSetNames : *derivesFromNames__) {
        auto* userSet = parent();
        for (const auto& userSetName : userSetNames) {
            auto& userSetSubsets = userSet->subsets();
            auto userSetIt = userSetSubsets.find(userSetName);
            if (userSetIt == userSetSubsets.end()) {
                failed = true;
                break;
            }
            userSet = userSetIt->second.get();
        }
        if (failed || userSet->postParentLoad()) {
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