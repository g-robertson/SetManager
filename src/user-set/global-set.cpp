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
    : UserSet(std::make_unique<std::set<std::string>>())
{}

std::string_view GlobalSet::name() const noexcept {
    return "GLOBAL";
}

void GlobalSet::saveMachineSubset(std::ostream&) noexcept {
}

void GlobalSet::loadMachineSubset(std::istream&) noexcept {
}

void GlobalSet::updateElements() noexcept {
}

const auto GLOBAL_CREATEABLE_SUBSET_MENU = StaticMenu<void, UserSet*, UserSet&, const std::string&>({
    {std::string(1, WordSet::type_), {"WordSet", WordSet::createSet}},
    {std::string(1, FauxWordSet::type_), {"FauxWordSet", FauxWordSet::createSet}},
    {std::string(1, DirectorySet::type_), {"DirectorySet", DirectorySet::createSet}},
    {std::string(1, IntersectionSet::type_), {"IntersectionSet", IntersectionSet::createSet}},
    {std::string(1, UnionSet::type_), {"UnionSet", UnionSet::createSet}},
    {std::string(1, DifferenceSet::type_), {"DifferenceSet", DifferenceSet::createSet}},
    {std::string(1, SymmetricDifferenceSet::type_), {"SymmetricDifferenceSet", SymmetricDifferenceSet::createSet}},
    {std::string(1, RelativeComplementSet::type_), {"RelativeComplementSet", RelativeComplementSet::createSet}},
    {std::string(UserSet::EXIT_KEYWORD), {"Exit", UserSet::EXIT_SET_MENU}}
});

const Menu<void, UserSet*, UserSet&, const std::string&>& GlobalSet::createableSubsetMenu() const noexcept {
    return GLOBAL_CREATEABLE_SUBSET_MENU;
}