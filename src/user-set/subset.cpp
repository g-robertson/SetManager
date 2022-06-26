#include "subset.hpp"

#include "word-set.hpp"

SubSet::SubSet(UserSet* parent, const std::string& name)
    : UserSet(parent), name_(name)
{}

std::string_view SubSet::setName() const {
    return name_;
}

const auto SUBSET_CREATEABLE_SUBSET_MENU = StaticMenu<void, std::unique_ptr<UserSet>, UserSet&, const std::string&>({
    {"W", {"WordSet", WordSet::createSet}},
    {UserSet::EXIT_KEYWORD, {"Exit", UserSet::EXIT_SET_MENU}}
});

const Menu<void, std::unique_ptr<UserSet>, UserSet&, const std::string&>& SubSet::createableSubsetMenu() const {
    return SUBSET_CREATEABLE_SUBSET_MENU;
}