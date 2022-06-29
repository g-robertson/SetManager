/*
    derivative-set.hpp

    DerivativeSet is a set that holds the properties of a subset, while also requiring elements of certain subsets of its parent set
    It is an abstract type of set due to not having any defined methods for gathering elements, complement elements, or whom it derives from
    Its purpose is to provide an interface for ensuring that all subsets necessary for a derivative subset are available before finishing its load
*/
#pragma once

#include "subset.hpp"

#include <unordered_map>
#include <set>

class DerivativeSet : public SubSet {
    public:
        DerivativeSet(UserSet* parent, const std::string& name) noexcept;

        const std::unordered_map<std::string, UserSet*>& derivesFrom() const noexcept;
        bool postParentLoad() noexcept override;
        virtual bool postPostParentLoad() noexcept;
    private:
        std::unordered_map<std::string, UserSet*> derivesFrom_;
        bool postParentLoading = false;
        bool postParentLoaded = false;
}; 