/*
    derivative-set.cpp

    DerivativeSet is a set that holds the properties of a subset, while also requiring elements of certain subsets of its parent set
    It is an abstract type of set due to not having any defined methods for gathering elements, complement elements, or whom it derives from
    Its purpose is to provide an interface for ensuring that all subsets necessary for a derivative subset are available before finishing its load
*/
#include "derivative-set.hpp"

DerivativeSet::DerivativeSet(UserSet* parent, const std::string& name) noexcept
    : SubSet(parent, name)
{}

const std::unordered_map<std::string, UserSet*>& DerivativeSet::derivesFrom() const noexcept {
    return derivesFrom_;
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
    for (auto& subset : derivesFrom()) {
        if (subset.second->postParentLoad()) {
            return true;
        }
    }
    postParentLoading = false;
    postParentLoaded = true;
    return postPostParentLoad();
}

bool DerivativeSet::postPostParentLoad() noexcept {
    return false;
}