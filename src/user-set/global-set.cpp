#include "global-set.hpp"

#include "word-set.hpp"
#include "directory-set.hpp"

std::string_view GlobalSet::setName() const {
    return "GLOBAL";
}

const std::set<std::string>* GlobalSet::elements() const {
    return nullptr;
}

const std::set<std::string>* GlobalSet::complementElements() const {
    return &NO_ELEMENTS;
}

const auto GLOBAL_CREATEABLE_SUBSET_MENU = StaticMenu<void, std::unique_ptr<UserSet>, UserSet&, const std::string&>({
    {"W", {"WordSet", WordSet::createSet}},
    {"D", {"DirectorySet", DirectorySet::createSet}},
    {UserSet::EXIT_KEYWORD, {"Exit", UserSet::EXIT_SET_MENU}}
});

const Menu<void, std::unique_ptr<UserSet>, UserSet&, const std::string&>& GlobalSet::createableSubsetMenu() const {
    return GLOBAL_CREATEABLE_SUBSET_MENU;
}