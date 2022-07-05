/*
    faux-word-set.hpp

    FauxWordSet is a type of subset that can contain any element that exists in the parent UserSet by selection, or entering the word,
    it also contains a set of words that do not exist in the parent UserSet that will be automatically added to its elements if the parent gains them

    Due to its possibilities of being capable of dealing with all words arbitrarily in any parent set, it can have any UserSet type as its parent 
*/
#include "faux-word-set.hpp"

#include "helpers.hpp"
#include "word-set.hpp"

#include <iostream>
#include <algorithm>

FauxWordSet::FauxWordSet(WordSet&& wordSet) noexcept 
    : SubSet(wordSet.parent(), std::string(wordSet.name())), elements_(std::move(wordSet.elements_))
{}

FauxWordSet::FauxWordSet(UserSet* parent, const std::string& name) noexcept
    : SubSet(parent, name)
{}

UserSet* FauxWordSet::createSet(UserSet& parent, const std::string& name) noexcept {
    return new FauxWordSet(&parent, name);
}

const auto FAUX_WORD_SET_MENU = ReinterpretMenu<FauxWordSet, UserSet, void>({
    {"A", {"Add a word", &FauxWordSet::addWord}},
    {"AX", {"Add a word from the parent set", &FauxWordSet::addParentWord}},
    {"R", {"Remove a word", &FauxWordSet::removeWord}},
    {"RX", {"Remove a word by number in this set", &FauxWordSet::removeContainedWord}},
    {"LE", {"List faux elements", &FauxWordSet::listFauxElements}},
    {"X", {"Exit set-specific options", &FauxWordSet::exitSetSpecificOptions}},
    {UserSet::EXIT_KEYWORD, {"Exit the program", &FauxWordSet::exitProgram}}
});

const Menu<UserSet, void>& FauxWordSet::setSpecificMenu() const noexcept {
    return FAUX_WORD_SET_MENU;
}

void FauxWordSet::addWord() noexcept {
    std::cout << "Specify a word you want to add to the set that exists in the parent set: ";
    std::string word;
    ignoreAll(std::cin);
    std::getline(std::cin, word);

    auto inserted = elements_.insert(word);
    if (!inserted.second) {
        std::cout << "That word was already in the set and was therefore not inserted\n";
    }
}

void FauxWordSet::addParentWord() noexcept {
    if (parent()->elements() == nullptr) {
        std::cout << "The parent has an infinite set of elements and cannot be specified from\n";
        return;
    }
    int count = 0;
    for (const auto& element : *parent()->elements()) {
        if (contains(element)) {
            continue;
        }
        ++count;
        std::cout << count << ". '" << element << "'\n";
    }

    int selection = 0;
    std::cout << "Select a number to add from the parent set, or any number not specified to exit: ";
    std::cin >> selection;
    if (selection > count || selection < 1) {
        return;
    }
    
    count = 0;
    for (const auto& element : *parent()->elements()) {
        if (contains(element)) {
            continue;
        }
        ++count;
        if (count == selection) {
            elements_.insert(element);
            return;
        }
    }
    std::cerr << "[FATAL ERROR]: Should not be able to get a selection that does not exist in the parent set";
    exitProgram();
}

void FauxWordSet::removeWord() noexcept {
    std::cout << "Specify a word you want to remove from the set: ";
    std::string word;
    ignoreAll(std::cin);
    std::getline(std::cin, word);

    removedElement(word, true);
}

void FauxWordSet::removeContainedWord() noexcept {
    if (elements_.size() == 0) {
        std::cout << "There are no words to select from to remove.\n";
        return;
    }

    int count = 0;
    for (const auto& element : elements_) {
        ++count;
        std::cout << count << ". '" << element << "'\n";
    }

    int selection = 0;
    std::cout << "Select a number to remove from the set, or any number not specified to exit: ";
    std::cin >> selection;
    if (selection > count || selection < 1) {
        return;
    }

    count = 0;
    for (const auto& element : elements_) {
        ++count;
        if (count == selection) {
            removedElement(element, true);
            return;
        }
    }
    std::cerr << "[FATAL ERROR]: Should not be able to get a selection that does not exist in the parent set";
    exitProgram();
}

void FauxWordSet::listFauxElements() noexcept {
    std::cout << "Element list\n"
              << std::string(80, '-') << '\n'
              << "This set contains faux elements:\n";
    for (const auto& element : elements_) {
        std::cout << '\'' << element << "'\n";
    }
    std::cout << std::string(80, '-') << '\n';
}

void FauxWordSet::saveMachineSubset(std::ostream& saveLocation) noexcept {
    saveLocation << elements_.size();
    for (const auto& element : elements_) {
        saveLocation << ' ' << element.size() << ' ' << element;
    }
}

void FauxWordSet::loadMachineSubset(std::istream& loadLocation) noexcept {
    size_t elementCount;
    loadLocation >> elementCount;
    for (; elementCount > 0; --elementCount) {
        size_t elementSize;
        loadLocation >> elementSize;
        // pre-creates a string of elementSize to read into
        std::string element;
        element.resize(elementSize);
        // skips over the space after size
        skipRead(loadLocation, 1);
        // reads all of the text into the element
        loadLocation.read(element.data(), elementSize);
        elements_.emplace(std::move(element));
    }
}

const std::set<std::string>* FauxWordSet::elements() noexcept {
    nonFauxElements_.clear();
    auto* parentElements = parent()->elements();
    std::set_intersection(elements_.begin(), elements_.end(), parentElements->begin(), parentElements->end(), std::inserter(nonFauxElements_, nonFauxElements_.begin()));
    return &nonFauxElements_;
} 

const std::set<std::string>* FauxWordSet::complementElements() noexcept {
    return nullptr;
}

void FauxWordSet::removedElement(const std::string& element, bool expected) noexcept {
    for (const auto& subset : subsets_) {
        subset.second->removedElement(element, expected);
    }
    elements_.erase(element);
}