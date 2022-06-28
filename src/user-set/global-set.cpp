/*
    global-set.cpp

    GlobalSet is a set that consists of all elements that exist.
    It cannot be created as a subset of any set due to having the requirement of encompassing all elements.
*/
#include "global-set.hpp"

#include "word-set.hpp"
#include "directory-set.hpp"

std::string_view GlobalSet::name() const {
    return "GLOBAL";
}

void GlobalSet::saveMachineSubset(std::ostream&) const {
}

void GlobalSet::loadMachineSubset(std::istream&) {
}

const std::set<std::string>* GlobalSet::elements() const {
    return nullptr;
}

const std::set<std::string>* GlobalSet::complementElements() const {
    return &NO_ELEMENTS;
}

const auto GLOBAL_CREATEABLE_SUBSET_MENU = StaticMenu<void, std::unique_ptr<UserSet>, UserSet&, const std::string&>({
    {std::string(1, WordSet::type_), {"WordSet", WordSet::createSet}},
    {std::string(1, DirectorySet::type_), {"DirectorySet", DirectorySet::createSet}},
    {UserSet::EXIT_KEYWORD, {"Exit", UserSet::EXIT_SET_MENU}}
});

const Menu<void, std::unique_ptr<UserSet>, UserSet&, const std::string&>& GlobalSet::createableSubsetMenu() const {
    return GLOBAL_CREATEABLE_SUBSET_MENU;
}