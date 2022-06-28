/*
    global-set.cpp

    GlobalSet is a set that consists of all elements that exist.
    It cannot be created as a subset of any set due to having the requirement of encompassing all elements.
*/
#include "global-set.hpp"

#include "word-set.hpp"
#include "faux-word-set.hpp"
#include "directory-set.hpp"

std::string_view GlobalSet::name() const noexcept {
    return "GLOBAL";
}

void GlobalSet::saveMachineSubset(std::ostream&) const noexcept {
}

void GlobalSet::loadMachineSubset(std::istream&) noexcept {
}

const std::set<std::string>* GlobalSet::elements() const noexcept {
    return nullptr;
}

const std::set<std::string>* GlobalSet::complementElements() const noexcept {
    return &NO_ELEMENTS;
}

const auto GLOBAL_CREATEABLE_SUBSET_MENU = StaticMenu<void, UserSet*, UserSet&, const std::string&>({
    {std::string(1, WordSet::type_), {"WordSet", WordSet::createSet}},
    {std::string(1, FauxWordSet::type_), {"FauxWordSet", FauxWordSet::createSet}},
    {std::string(1, DirectorySet::type_), {"DirectorySet", DirectorySet::createSet}},
    {UserSet::EXIT_KEYWORD, {"Exit", UserSet::EXIT_SET_MENU}}
});

const Menu<void, UserSet*, UserSet&, const std::string&>& GlobalSet::createableSubsetMenu() const noexcept {
    return GLOBAL_CREATEABLE_SUBSET_MENU;
}