/*
    derivative-set.cpp

    DerivativeSet is a set that holds the properties of a subset, while also requiring elements of certain subsets of its parent set
    It is an abstract type of set due to not having any defined methods for gathering elements, complement elements, or whom it derives from
    Its purpose is to provide an interface for ensuring that all subsets necessary for a derivative subset are available before finishing its load

    Note: It is undefined behavior to instantiate this class with the parent, name constructor and then not run postParentLoad() hook
*/
#include "derivative-set.hpp"

DerivativeSet::DerivativeSet(UserSet* parent, const std::string& name, std::initializer_list<UserSet*> userSets) noexcept
    : SubSet(parent, name), derivesFrom_(new std::vector<UserSet*>(userSets))
{}

// Note: It is undefined behavior to instantiate this class with the parent, name constructor and then not run postParentLoad() hook
DerivativeSet::DerivativeSet(UserSet* parent, const std::string& name) noexcept
    : SubSet(parent, name), derivesFromNames_(new std::vector<std::vector<std::string>>)
{}

DerivativeSet::~DerivativeSet() noexcept {
    delete derivesFrom_;
}

std::vector<UserSet*>& DerivativeSet::derivesFrom() noexcept {
    return *derivesFrom_;
}

const std::vector<UserSet*>& DerivativeSet::derivesFrom() const noexcept {
    return *derivesFrom_;
}

void DerivativeSet::saveMachineSubset(std::ostream& saveLocation) noexcept {
    saveLocation << derivesFrom_->size();
    for (const auto* userSet : *derivesFrom_) {
        std::stringstream nestedSubsetsWrite;
        int nestedCount = -1;
        for (; userSet != parent(); userSet = userSet->parent()) {
            nestedSubsetsWrite << ' ' << userSet->name().size() << ' ' << userSet->name();
            ++nestedCount;
        }
        saveLocation << ' ' << nestedCount << nestedSubsetsWrite.str();
    }
    saveMachineDerivativeSubset(saveLocation);
}

void DerivativeSet::loadMachineSubset(std::istream& loadLocation) noexcept {
    int userSetsCount;
    loadLocation >> userSetsCount;
    derivesFromNames_->reserve(userSetsCount);
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
        derivesFromNames_->push_back(std::move(userSetNames));
    }
    loadMachineDerivativeSubset(loadLocation);
}

void DerivativeSet::saveMachineDerivativeSubset(std::ostream&) noexcept {
}

void DerivativeSet::loadMachineDerivativeSubset(std::istream&) noexcept {
}

void DerivativeSet::postParentLoad() noexcept(false) {
    // if already loaded, then don't load again
    if (postParentLoaded) {
        return;
    }
    // if in process of loading, then there is a recursive set somewhere, which is not allowed
    // also, this error cannot happen unless the set has already begun postParentLoad (i.e. it is already valid)
    if (postParentLoading) {
        throw std::logic_error("Recursive parent loading detected");
    }
    // immediately make it valid after calling postParentLoad
    std::vector<std::vector<std::string>> derivesFromNames = std::move(*derivesFromNames_);
    delete derivesFromNames_;
    derivesFrom_ = new std::vector<UserSet*>;
    derivesFrom_->reserve(derivesFromNames.size());
    postParentLoading = true;

    // adds all the nested names in derivesFromNames_ to userSets
    for (const auto& userSetNames : derivesFromNames) {
        auto* userSet = parent();
        for (const auto& userSetName : userSetNames) {
            auto& userSetSubsets = userSet->subsets();
            auto userSetIt = userSetSubsets.find(userSetName);
            if (userSetIt == userSetSubsets.end()) {
                throw std::logic_error(std::string("User set '") + userSetName + "' included in derivative set could not be found");
            }
            userSet = userSetIt->second.get();
        }
        userSet->postParentLoad();
        derivesFrom_->push_back(userSet);
    }

    postParentLoading = false;
    postParentLoaded = true;
    return postPostParentLoad();
}

void DerivativeSet::postPostParentLoad() noexcept(false) {
}