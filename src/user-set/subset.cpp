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
    const pstring& name,
    std::unique_ptr<std::set<pstring>> elements,
    std::unique_ptr<std::set<pstring>> complementElements
) noexcept
    : UserSet(parent, std::move(elements), std::move(complementElements)), name_(name)
{}

pstring_view SubSet::name() const noexcept {
    return name_;
}

const auto SUBSET_CREATEABLE_SUBSET_MENU = StaticMenu<void, UserSet*, UserSet&, const pstring&>({
    {pstring(1, WordSet::type_), {pliteral("WordSet"), WordSet::createSet}},
    {pstring(1, FauxWordSet::type_), {pliteral("FauxWordSet"), FauxWordSet::createSet}},
    {pstring(1, IntersectionSet::type_), {pliteral("IntersectionSet"), IntersectionSet::createSet}},
    {pstring(1, UnionSet::type_), {pliteral("UnionSet"), UnionSet::createSet}},
    {pstring(1, DifferenceSet::type_), {pliteral("DifferenceSet"), DifferenceSet::createSet}},
    {pstring(1, SymmetricDifferenceSet::type_), {pliteral("SymmetricDifferenceSet"), SymmetricDifferenceSet::createSet}},
    {pstring(1, RelativeComplementSet::type_), {pliteral("RelativeComplementSet"), RelativeComplementSet::createSet}},
    {UserSet::EXIT_KEYWORD, {pliteral("Exit"), UserSet::EXIT_SET_MENU}}
});

// Queries which subsets are able to be created from SubSet type UserSets
const Menu<void, UserSet*, UserSet&, const pstring&>& SubSet::createableSubsetMenu() const noexcept {
    return SUBSET_CREATEABLE_SUBSET_MENU;
}