/*
    word-set.cpp

    WordSet is a type of subset that can contain any element that exists in the parent UserSet by selection, or entering the word.

    Due to its possibilities of being capable of dealing with all words arbitrarily in any parent set, it can have any UserSet type as its parent 
*/
#include "word-set.hpp"

#include "helpers.hpp"
#include "faux-word-set.hpp"

#include <iostream>

WordSet::WordSet(UserSet* parent, const pstring& name) noexcept
    : SubSet(parent, name, std::make_unique<std::set<pstring>>())
{}

UserSet* WordSet::createSet(UserSet& parent, const pstring& name) noexcept {
    return new WordSet(&parent, name);
}

const auto WORD_SET_MENU = ReinterpretMenu<WordSet, UserSet, void>({
    {pliteral("A"), {pliteral("Add a word"), &WordSet::addWord}},
    {pliteral("AX"), {pliteral("Add a word from the parent set"), &WordSet::addParentWord}},
    {pliteral("R"), {pliteral("Remove a word"), &WordSet::removeWord}},
    {pliteral("RX"), {pliteral("Remove a word by number in this set"), &WordSet::removeContainedWord}},
    {pliteral("X"), {pliteral("Exit set-specific options"), &WordSet::exitSetSpecificOptions}},
    {pstring(UserSet::EXIT_KEYWORD), {pliteral("Exit the program"), &WordSet::exitProgram}}
});

const Menu<UserSet, void>& WordSet::setSpecificMenu() const noexcept {
    return WORD_SET_MENU;
}

void WordSet::addWord() noexcept {
    pcout << "Specify a word you want to add to the set that exists in the parent set: ";
    pstring word;
    pCinIgnoreAll();;
    pCinGetLine(word);

    if (!parent()->contains(word)) {
        pcout << "That word is not in the parent set, and would make this not be a subset, and was therefore not inserted\n";
        return;
    }
    auto inserted = addElement(word);
    if (!inserted) {
        pcout << "That word was already in the set and was therefore not inserted\n";
    }
}

void WordSet::addParentWord() noexcept {
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

void WordSet::removeWord() noexcept {
    pcout << "Specify a word you want to remove from the set: ";
    pstring word;
    pCinIgnoreAll();;
    pCinGetLine(word);

    removedElement(word, true);
}

void WordSet::removeContainedWord() noexcept {
    if (elements_->size() == 0) {
        pcout << "There are no words to select from to remove.\n";
        return;
    }

    int count = 0;
    for (const auto& element : *elements_) {
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
    for (const auto& element : *elements_) {
        ++count;
        if (count == selection) {
            removedElement(element, true);
            return;
        }
    }
    std::cerr << "[FATAL ERROR]: Should not be able to get a selection that does not exist in the parent set";
    exitProgram();
}

void WordSet::saveMachineSubset(std::ostream& saveLocation) noexcept {
    saveLocation << elements()->size();
    for (const auto& element : *elements()) {
        std::string genericElement = genericStringFromPString(element);
        saveLocation << ' ' << genericElement.size() << ' ' << genericElement;
    }
}

void WordSet::loadMachineSubset(std::istream& loadLocation) noexcept {
    size_t elementCount;
    loadLocation >> elementCount;
    for (; elementCount > 0; --elementCount) {
        size_t elementSize;
        loadLocation >> elementSize;
        // pre-creates a string of elementSize to read into
        std::string genericStringElement;
        genericStringElement.resize(elementSize);
        // skips over the space after size
        skipRead(loadLocation, 1);
        // reads all of the text into the element
        loadLocation.read(genericStringElement.data(), elementSize);
        pstring element = genericStringToPString(std::move(genericStringElement));
        elements_->emplace(std::move(element));
    }
}

void WordSet::postParentLoad() noexcept(false) {
    for (const auto& element : *elements_) {
        if (becomingFaux != nullptr) {
            break;
        }

        // if parent doesn't contain the element then issue a warning
        if (!parent()->contains(element)) {
            handleUnexpectedWordRemoval(element);
            continue;
        }
    }
}

void WordSet::updateElements() noexcept {
}


bool WordSet::addElement(const pstring& element) noexcept {
    return elements_->insert(element).second;
}

void WordSet::removedElement(const pstring& element, bool expected) noexcept {
    if (!expected) {
        handleUnexpectedWordRemoval(element);
    }
    for (const auto& subset : subsets_) {
        subset.second->removedElement(element, true);
    }
    elements_->erase(element);
}

void WordSet::handleUnexpectedWordRemoval(const pstring& element) noexcept {
    pcout << "The element '" << element << "' was attempted to be unexpectedly removed from the '" << name() << "' nested word set.\n"
              << "you may either delete this element, exit the program without saving, or this word set can be substituted with an Faux-Wordset which allows faux non-subsetted words\n"
              << "Enter [D] to delete the element, [E] to exit the program without saving, or [F] to substitute the WordSet with a Faux-WordSet: ";
    pstring input;
    do {
        pcin >> input;
        if (ptoupper(input[0]) == 'D') {
            return;
        } else if (ptoupper(input[0]) == 'E') {
            exit(0);
        } else if (ptoupper(input[0]) == 'F') {
            elements_->insert(element);

            parent()->onQueryRemove = this;
            auto fauxWordSet = std::make_unique<FauxWordSet>(std::move(*this));
            becomingFaux = fauxWordSet.get();
            parent()->onQueryAdd = std::move(fauxWordSet);
            return;
        }
    } while (true);
}