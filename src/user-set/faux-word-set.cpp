/*
    faux-word-set.hpp

    FauxWordSet is a type of subset that can contain any element that exists in the parent UserSet by selection, or entering the word,
    it also contains a set of fauxElements that do not exist in the parent UserSet that will be automatically added to its elements if the parent gains them

    Due to its possibilities of being capable of dealing with all fauxElements arbitrarily in any parent set, it can have any UserSet type as its parent 
*/
#include "faux-word-set.hpp"

#include "helpers.hpp"
#include "word-set.hpp"

#include <iostream>
#include <algorithm>

FauxWordSet::FauxWordSet(WordSet&& wordSet) noexcept 
    : SubSet(wordSet.parent(), std::string(wordSet.name()), std::move(wordSet.elements_))
{}

FauxWordSet::FauxWordSet(UserSet* parent, const std::string& name) noexcept
    : SubSet(parent, name, std::make_unique<std::set<std::string>>())
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
    {std::string(UserSet::EXIT_KEYWORD), {"Exit the program", &FauxWordSet::exitProgram}}
});

const Menu<UserSet, void>& FauxWordSet::setSpecificMenu() const noexcept {
    return FAUX_WORD_SET_MENU;
}

void FauxWordSet::addWord() noexcept {
    nowide::cout << "Specify a word you want to add to the set that exists in the parent set: ";
    std::string word;
    ignoreAll(nowide::cin);;
    std::getline(nowide::cin, word);

    auto inserted = addElement(word);
    if (!inserted) {
        nowide::cout << "That word was already in the set and was therefore not inserted\n";
    }
}

void FauxWordSet::addParentWord() noexcept {
    if (parent()->elements() == nullptr) {
        nowide::cout << "The parent has an infinite set of elements and cannot be specified from\n";
        return;
    }
    int count = 0;
    for (const auto& element : *parent()->elements()) {
        if (contains(element)) {
            continue;
        }
        ++count;
        nowide::cout << count << ". '" << element << "'\n";
    }

    int selection = 0;
    nowide::cout << "Select a number to add from the parent set, or any number not specified to exit: ";
    nowide::cin >> selection;
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
            addElement(element);
            return;
        }
    }
    std::cerr << "[FATAL ERROR]: Should not be able to get a selection that does not exist in the parent set";
    exitProgram();
}

void FauxWordSet::removeWord() noexcept {
    nowide::cout << "Specify a word you want to remove from the set: ";
    std::string word;
    ignoreAll(nowide::cin);;
    std::getline(nowide::cin, word);

    removedElement(word, true);
}

void FauxWordSet::removeContainedWord() noexcept {
    if (fauxElements.size() == 0) {
        nowide::cout << "There are no fauxElements to select from to remove.\n";
        return;
    }

    int count = 0;
    for (const auto& element : fauxElements) {
        ++count;
        nowide::cout << count << ". '" << element << "'\n";
    }

    int selection = 0;
    nowide::cout << "Select a number to remove from the set, or any number not specified to exit: ";
    nowide::cin >> selection;
    if (selection > count || selection < 1) {
        return;
    }

    count = 0;
    for (const auto& element : fauxElements) {
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
    nowide::cout << "Element list\n"
              << std::string(80, '-') << '\n'
              << "This set contains faux elements:\n";
    for (const auto& element : fauxElements) {
        nowide::cout << '\'' << element << "'\n";
    }
    nowide::cout << std::string(80, '-') << '\n';
}

void FauxWordSet::saveMachineSubset(std::ostream& saveLocation) noexcept {
    saveLocation << fauxElements.size();
    for (const auto& element : fauxElements) {
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
        fauxElements.emplace(std::move(element));
    }
}

void FauxWordSet::updateElements() noexcept {
    elements_->clear();
    auto* parentElements = parent()->elements();
    if (parentElements != nullptr) {
        std::set_intersection(fauxElements.begin(), fauxElements.end(), parentElements->begin(), parentElements->end(), std::inserter(*elements_, elements_->begin()));
    } else {
        auto* parentComplementElements = parent()->complementElements();
        std::set_difference(fauxElements.begin(), fauxElements.end(), parentComplementElements->begin(), parentComplementElements->end(), std::inserter(*elements_, elements_->begin()));
    }
} 

bool FauxWordSet::addElement(const std::string& element) noexcept {
    bool added = fauxElements.insert(element).second;
    updateElements();
    return added;
}

void FauxWordSet::removedElement(const std::string& element, bool expected) noexcept {
    for (const auto& subset : subsets_) {
        subset.second->removedElement(element, expected);
    }
    fauxElements.erase(element);
}