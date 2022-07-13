/*
    subset.hpp

    SubSet is a set that contains elements that exist within a parent set
    It is an abstract type of set due to not having any defined methods for gathering elements, or complement elements
    Its purpose is to make assertions about what a subset of a parent set must necessarily include
*/
#pragma once

#include "user-set.hpp"

class SubSet : public UserSet {
    public:
        SubSet(
            UserSet* parent,
            const std::string& name,
            std::unique_ptr<std::set<std::string>> elements = std::unique_ptr<std::set<std::string>>(),
            std::unique_ptr<std::set<std::string>> complementElements = std::unique_ptr<std::set<std::string>>()
        ) noexcept;
        // Must exist in all derived SubSets for creation
        // static UserSet* createSet(UserSet& parent, const std::string& name) noexcept;

        std::string_view name() const noexcept override;
    private:
        // Queries which subsets are able to be created from SubSet type UserSets
        const Menu<void, UserSet*, UserSet&, const std::string&>& createableSubsetMenu() const noexcept override;

        std::string name_;
};