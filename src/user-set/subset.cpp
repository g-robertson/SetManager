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
#include "union-set.hpp"
#include "difference-set.hpp"
#include "symmetric-difference-set.hpp"
#include "relative-complement-set.hpp"

SubSet::SubSet(
    UserSet* parent,
    const std::string& name,
    std::unique_ptr<std::set<std::string>> elements,
    std::unique_ptr<std::set<std::string>> complementElements
) noexcept
    : UserSet(parent, std::move(elements), std::move(complementElements)), name_(name)
{}

std::string_view SubSet::name() const noexcept {
    return name_;
}

const auto SUBSET_CREATEABLE_SUBSET_MENU = StaticMenu<void, UserSet*, UserSet&, const std::string&>({
    {std::string(1, WordSet::type_), {"WordSet", WordSet::createSet}},
    {std::string(1, FauxWordSet::type_), {"FauxWordSet", FauxWordSet::createSet}},
    {std::string(1, IntersectionSet::type_), {"IntersectionSet", IntersectionSet::createSet}},
    {std::string(1, UnionSet::type_), {"UnionSet", UnionSet::createSet}},
    {std::string(1, DifferenceSet::type_), {"DifferenceSet", DifferenceSet::createSet}},
    {std::string(1, SymmetricDifferenceSet::type_), {"SymmetricDifferenceSet", SymmetricDifferenceSet::createSet}},
    {std::string(1, RelativeComplementSet::type_), {"RelativeComplementSet", RelativeComplementSet::createSet}},
    {UserSet::EXIT_KEYWORD, {"Exit", UserSet::EXIT_SET_MENU}}
});

// Queries which subsets are able to be created from SubSet type UserSets
const Menu<void, UserSet*, UserSet&, const std::string&>& SubSet::createableSubsetMenu() const noexcept {
    return SUBSET_CREATEABLE_SUBSET_MENU;
}