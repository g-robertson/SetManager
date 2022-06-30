/*
    word-set.cpp

    WordSet is a type of subset that can contain any element that exists in the parent UserSet by selection, or entering the word.

    Due to its possibilities of being capable of dealing with all words arbitrarily in any parent set, it can have any UserSet type as its parent 
*/
#include "word-set.hpp"

#include "helpers.hpp"
#include "faux-word-set.hpp"

#include <iostream>

WordSet::WordSet(UserSet* parent, const std::string& name) noexcept
    : SubSet(parent, name)
{}

UserSet* WordSet::createSet(UserSet& parent, const std::string& name) noexcept {
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

const Menu<UserSet, void>& WordSet::setSpecificMenu() const noexcept {
    return WORD_SET_MENU;
}

void WordSet::addWord() noexcept {
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

void WordSet::addParentWord() noexcept {
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
    std::cerr << "Impossible error: [Should not be able to get a selection that does not exist in the parent set]";
    exitProgram();
}

void WordSet::removeWord() noexcept {
    std::cout << "Specify a word you want to remove from the set: ";
    std::string word;
    ignoreAll(std::cin);
    std::getline(std::cin, word);

    removedElement(word, true);
}

void WordSet::removeContainedWord() noexcept {
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
    std::cerr << "Impossible error: [Should not be able to get a selection that does not exist in the parent set]";
    exitProgram();
}

void WordSet::saveMachineSubset(std::ostream& saveLocation) noexcept {
    saveLocation << elements()->size();
    for (const auto& element : *elements()) {
        saveLocation << ' ' << element.size() << ' ' << element;
    }
}

void WordSet::loadMachineSubset(std::istream& loadLocation) noexcept {
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
        
        if (becomingFaux != nullptr) {
            becomingFaux->elements_.emplace(std::move(element));
            continue;
        }
        // if parent doesn't contain the element then issue a warning
        if (!parent->contains(element)) {
            handleUnexpectedWordRemoval(element);
            continue;
        }
        elements_.emplace(std::move(element));
    }
}

const std::set<std::string>* WordSet::elements() const noexcept {
    return &elements_;
} 

const std::set<std::string>* WordSet::complementElements() const noexcept {
    return nullptr;
}

void WordSet::removedElement(const std::string& element, bool expected) noexcept {
    if (!expected) {
        handleUnexpectedWordRemoval(element);
    }
    for (const auto& subset : subsets_) {
        subset.second->removedElement(element, true);
    }
    elements_.erase(element);
}

void WordSet::handleUnexpectedWordRemoval(const std::string& element) noexcept {
    std::cout << "The element '" << element << "' was attempted to be unexpectedly removed from the '" << name() << "' nested word set.\n"
              << "you may either delete this element, exit the program without saving, or this word set can be substituted with an Faux-Wordset which allows faux non-subsetted words\n"
              << "Enter [D] to delete the element, [E] to exit the program without saving, or [F] to substitute the WordSet with a Faux-WordSet: ";
    std::string input;
    do {
        std::cin >> input;
        if (std::toupper(input[0], std::locale()) == 'D') {
            return;
        } else if (std::toupper(input[0], std::locale()) == 'E') {
            exit(0);
        } else if (std::toupper(input[0], std::locale()) == 'F') {
            elements_.insert(element);

            parent->onQueryRemove = this;
            auto fauxWordSet = std::make_unique<FauxWordSet>(std::move(*this));
            becomingFaux = fauxWordSet.get();
            parent->onQueryAdd = std::move(fauxWordSet);
            return;
        }
    } while (true);
}