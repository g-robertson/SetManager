/*
    subset.cpp

    SubSet is a set that contains elements that exist within a parent set
    It is an abstract type of set due to not having any defined methods for gathering elements, or complement elements
    Its purpose is to make assertions about what a subset of a parent set must necessarily include
*/
#include "subset.hpp"

#include "word-set.hpp"

SubSet::SubSet(UserSet* parent, const std::string& name)
    : UserSet(parent), name_(name)
{}

std::string_view SubSet::name() const {
    return name_;
}

const auto SUBSET_CREATEABLE_SUBSET_MENU = StaticMenu<void, UserSet*, UserSet&, const std::string&>({
    {std::string(1, WordSet::type_), {"WordSet", WordSet::createSet}},
    {UserSet::EXIT_KEYWORD, {"Exit", UserSet::EXIT_SET_MENU}}
}, nullptr);

// Queries which subsets are able to be created from SubSet type UserSets
const Menu<void, UserSet*, UserSet&, const std::string&>& SubSet::createableSubsetMenu() const {
    return SUBSET_CREATEABLE_SUBSET_MENU;
}