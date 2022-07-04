/*
    subset.cpp

    SubSet is a set that contains elements that exist within a parent set
    It is an abstract type of set due to not having any defined methods for gathering elements, or complement elements
    Its purpose is to make assertions about what a subset of a parent set must necessarily include
*/
#include "subset.hpp"

#include "word-set.hpp"
#include "faux-word-set.hpp"
#include "intersection-set.hpp"

SubSet::SubSet(UserSet* parent, const std::string& name) noexcept
    : UserSet(parent), name_(name)
{}

std::string_view SubSet::name() const noexcept {
    return name_;
}

const auto SUBSET_CREATEABLE_SUBSET_MENU = StaticMenu<void, UserSet*, UserSet&, const std::string&>({
    {std::string(1, WordSet::type_), {"WordSet", WordSet::createSet}},
    {std::string(1, FauxWordSet::type_), {"FauxWordSet", FauxWordSet::createSet}},
    {std::string(1, IntersectionSet::type_), {"IntersectionSet", IntersectionSet::createSet}},
    {UserSet::EXIT_KEYWORD, {"Exit", UserSet::EXIT_SET_MENU}}
});

// Queries which subsets are able to be created from SubSet type UserSets
const Menu<void, UserSet*, UserSet&, const std::string&>& SubSet::createableSubsetMenu() const noexcept {
    return SUBSET_CREATEABLE_SUBSET_MENU;
}