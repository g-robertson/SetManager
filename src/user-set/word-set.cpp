/*
    word-set.cpp

    WordSet is a type of subset that can contain any element that exists in the parent UserSet by selection, or entering the word.

    Due to its possibilities of being capable of dealing with all words arbitrarily in any parent set, it can have any UserSet type as its parent 
*/
#include "word-set.hpp"

#include "helpers.hpp"

#include <iostream>

WordSet::WordSet(UserSet* parent, const std::string& name)
    : SubSet(parent, name)
{}

UserSet* WordSet::createSet(UserSet& parent, const std::string& name) {
    return new WordSet(&parent, name);
}

const auto WORD_SET_MENU = ReinterpretMenu<WordSet, UserSet, void>({
    {"A", {"Add a word", &WordSet::addWord}},
    {"AX", {"Add a word from the parent set", &WordSet::addParentWord}},
    {"R", {"Remove a word", &WordSet::removeWord}},
    {"RX", {"Remove a word by number in this set", &WordSet::removeContainedWord}},
    {"X", {"Exit set-specific options", &WordSet::exitSetSpecificOptions}},
    {UserSet::EXIT_KEYWORD, {"Exit the program", &WordSet::exitProgram}}
});

const Menu<UserSet, void>& WordSet::setSpecificMenu() const {
    return WORD_SET_MENU;
}

void WordSet::addWord() {
    std::cout << "Specify a word you want to add to the set that exists in the parent set: ";
    std::string word;
    ignoreAll(std::cin);
    std::getline(std::cin, word);

    if (!parent->contains(word)) {
        std::cout << "That word is not in the parent set, and would make this not be a subset, and was therefore not inserted\n";
        return;
    }
    auto inserted = elements_.insert(word);
    if (!inserted.second) {
        std::cout << "That word was already in the set and was therefore not inserted\n";
    }
}

void WordSet::addParentWord() {
    if (parent->elements() == nullptr) {
        std::cout << "The parent has an infinite set of elements and cannot be specified from\n";
        return;
    }
    int count = 0;
    for (const auto& element : *parent->elements()) {
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
    for (const auto& element : *parent->elements()) {
        if (contains(element)) {
            continue;
        }
        ++count;
        if (count == selection) {
            elements_.insert(element);
            return;
        }
    }
    throw std::logic_error("Unappearable [Should not be able to get a selection that does not exist in the parent set]");
}

void WordSet::removeWord() {
    std::cout << "Specify a word you want to remove from the set: ";
    std::string word;
    ignoreAll(std::cin);
    std::getline(std::cin, word);

    removedElement(word, true);
}

void WordSet::removeContainedWord() {
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
    throw std::logic_error("Unappearable [Should not be able to get a selection that does not exist in the parent set]");
}

void WordSet::saveMachineSubset(std::ostream& saveLocation) const {
    saveLocation << elements()->size();
    for (const auto& element : *elements()) {
        saveLocation << ' ' << element.size() << ' ' << element;
    }
}

void WordSet::loadMachineSubset(std::istream& loadLocation) {
    size_t elementCount;
    loadLocation >> elementCount;
    for (; elementCount > 0; --elementCount) {
        size_t elementSize;
        loadLocation >> elementSize;
        // pre-creates a string of elementSize to read into
        auto element = std::string(elementSize, '0');
        // skips over the space after size
        skipRead(loadLocation, 1);
        // reads all of the text into the element
        loadLocation.read(element.data(), elementSize);
        // if parent doesn't contain the element then issue a warning
        if (!parent->contains(element)) {
            throw std::logic_error("Partial load, load-resolveable unaccounted for. [WordSet parent element not found warning solveable]");
        }
        elements_.emplace(std::move(element));
    }
}

const std::set<std::string>* WordSet::elements() const {
    return &elements_;
} 

const std::set<std::string>* WordSet::complementElements() const {
    return &NO_ELEMENTS;
}

void WordSet::removedElement(const std::string& element, bool expected) {
    if (!expected) {
        throw std::logic_error("Resolveable unaccounted for. [WordSet had a word removed unexpectedly]");
    }
    for (const auto& subset : subsets) {
        subset.second->removedElement(element, true);
    }
    elements_.erase(element);
}