/*
    global-set.cpp

    GlobalSet is a set that consists of all elements that exist.
    It cannot be created as a subset of any set due to having the requirement of encompassing all elements.
*/
#include "global-set.hpp"

#include "word-set.hpp"
#include "faux-word-set.hpp"
#include "directory-set.hpp"
#include "intersection-set.hpp"
#include "union-set.hpp"
#include "difference-set.hpp"
#include "symmetric-difference-set.hpp"
#include "relative-complement-set.hpp"

GlobalSet::GlobalSet()
    : UserSet(std::make_unique<std::set<pstring>>())
{}

pstring_view GlobalSet::name() const noexcept {
    return pliteral("GLOBAL");
}

void GlobalSet::saveMachineSubset(std::ostream&) noexcept {
}

void GlobalSet::loadMachineSubset(std::istream&) noexcept {
}

void GlobalSet::updateElements() noexcept {
}

const auto GLOBAL_CREATEABLE_SUBSET_MENU = StaticMenu<void, UserSet*, UserSet&, const pstring&>({
    {pstring(1, WordSet::type_), {pliteral("WordSet"), WordSet::createSet}},
    {pstring(1, FauxWordSet::type_), {pliteral("FauxWordSet"), FauxWordSet::createSet}},
    {pstring(1, DirectorySet::type_), {pliteral("DirectorySet"), DirectorySet::createSet}},
    {pstring(1, IntersectionSet::type_), {pliteral("IntersectionSet"), IntersectionSet::createSet}},
    {pstring(1, UnionSet::type_), {pliteral("UnionSet"), UnionSet::createSet}},
    {pstring(1, DifferenceSet::type_), {pliteral("DifferenceSet"), DifferenceSet::createSet}},
    {pstring(1, SymmetricDifferenceSet::type_), {pliteral("SymmetricDifferenceSet"), SymmetricDifferenceSet::createSet}},
    {pstring(1, RelativeComplementSet::type_), {pliteral("RelativeComplementSet"), RelativeComplementSet::createSet}},
    {UserSet::EXIT_KEYWORD, {pliteral("Exit"), UserSet::EXIT_SET_MENU}}
});

const Menu<void, UserSet*, UserSet&, const pstring&>& GlobalSet::createableSubsetMenu() const noexcept {
    return GLOBAL_CREATEABLE_SUBSET_MENU;
}