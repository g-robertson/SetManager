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
    : SubSet(wordSet.parent(), pstring(wordSet.name()), std::move(wordSet.elements_))
{}

FauxWordSet::FauxWordSet(UserSet* parent, const pstring& name) noexcept
    : SubSet(parent, name, std::make_unique<std::set<pstring>>())
{}

UserSet* FauxWordSet::createSet(UserSet& parent, const pstring& name) noexcept {
    return new FauxWordSet(&parent, name);
}

const auto FAUX_WORD_SET_MENU = ReinterpretMenu<FauxWordSet, UserSet, void>({
    {pliteral("A"), {pliteral("Add a word"), &FauxWordSet::addWord}},
    {pliteral("AX"), {pliteral("Add a word from the parent set"), &FauxWordSet::addParentWord}},
    {pliteral("R"), {pliteral("Remove a word"), &FauxWordSet::removeWord}},
    {pliteral("RX"), {pliteral("Remove a word by number in this set"), &FauxWordSet::removeContainedWord}},
    {pliteral("LE"), {pliteral("List faux elements"), &FauxWordSet::listFauxElements}},
    {pliteral("X"), {pliteral("Exit set-specific options"), &FauxWordSet::exitSetSpecificOptions}},
    {pstring(UserSet::EXIT_KEYWORD), {pliteral("Exit the program"), &FauxWordSet::exitProgram}}
});

const Menu<UserSet, void>& FauxWordSet::setSpecificMenu() const noexcept {
    return FAUX_WORD_SET_MENU;
}

void FauxWordSet::addWord() noexcept {
    pcout << "Specify a word you want to add to the set that exists in the parent set: ";
    pstring word;
    pCinIgnoreAll();;
    pCinGetLine(word);

    auto inserted = addElement(word);
    if (!inserted) {
        pcout << "That word was already in the set and was therefore not inserted\n";
    }
}

void FauxWordSet::addParentWord() noexcept {
    if (parent()->elements() == nullptr) {
        pcout << "The parent has an infinite set of elements and cannot be specified from\n";
        return;
    }
    int count = 0;
    for (const auto& element : *parent()->elements()) {
        if (contains(element)) {
            continue;
        }
        ++count;
        pcout << count << ". '" << element << "'\n";
    }

    int selection = 0;
    pcout << "Select a number to add from the parent set, or any number not specified to exit: ";
    pcin >> selection;
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
    pcout << "Specify a word you want to remove from the set: ";
    pstring word;
    pCinIgnoreAll();;
    pCinGetLine(word);

    removedElement(word, true);
}

void FauxWordSet::removeContainedWord() noexcept {
    if (fauxElements.size() == 0) {
        pcout << "There are no fauxElements to select from to remove.\n";
        return;
    }

    int count = 0;
    for (const auto& element : fauxElements) {
        ++count;
        pcout << count << ". '" << element << "'\n";
    }

    int selection = 0;
    pcout << "Select a number to remove from the set, or any number not specified to exit: ";
    pcin >> selection;
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
    pcout << "Element list\n"
              << pstring(80, '-') << '\n'
              << "This set contains faux elements:\n";
    for (const auto& element : fauxElements) {
        pcout << '\'' << element << "'\n";
    }
    pcout << pstring(80, '-') << '\n';
}

void FauxWordSet::saveMachineSubset(std::ostream& saveLocation) noexcept {
    saveLocation << fauxElements.size();
    for (const auto& element : fauxElements) {
        std::string genericElement = genericStringFromPString(element);
        saveLocation << ' ' << genericElement.size() << ' ' << genericElement;
    }
}

void FauxWordSet::loadMachineSubset(std::istream& loadLocation) noexcept {
    size_t elementCount;
    loadLocation >> elementCount;
    for (; elementCount > 0; --elementCount) {
        size_t elementSize;
        loadLocation >> elementSize;
        // pre-creates a string of elementSize to read into
        std::string genericElement;
        genericElement.resize(elementSize);
        // skips over the space after size
        skipRead(loadLocation, 1);
        // reads all of the text into the element
        loadLocation.read(genericElement.data(), elementSize);
        pstring element = genericStringToPString(std::move(genericElement));
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

bool FauxWordSet::addElement(const pstring& element) noexcept {
    bool added = fauxElements.insert(element).second;
    updateElements();
    return added;
}

void FauxWordSet::removedElement(const pstring& element, bool expected) noexcept {
    for (const auto& subset : subsets_) {
        subset.second->removedElement(element, expected);
    }
    fauxElements.erase(element);
}