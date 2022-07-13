/*
    faux-word-set.hpp

    FauxWordSet is a type of subset that can contain any element that exists in the parent UserSet by selection, or entering the word,
    it also contains a set of words that do not exist in the parent UserSet that will be automatically added to its elements if the parent gains them

    Due to its possibilities of being capable of dealing with all words arbitrarily in any parent set, it can have any UserSet type as its parent 
*/
#pragma once 

#include "subset.hpp"

class WordSet;

class FauxWordSet : public SubSet {
    public:
        FauxWordSet(WordSet&& wordSet) noexcept;
        FauxWordSet(UserSet* parent, const std::string& name) noexcept;
        // #region SubSet public members override 
        static UserSet* createSet(UserSet& parent, const std::string& name) noexcept;
        // #endregion 
        // #region UserSet public members override 
        static constexpr char type_ = 'F';
        char type() const noexcept override  { return type_; }

        void saveMachineSubset(std::ostream& saveLocation) noexcept override;
        void loadMachineSubset(std::istream& loadLocation) noexcept override;
        void updateElements() noexcept override;
        // #endregion 

        void addWord() noexcept;
        void addParentWord() noexcept;
        void removeWord() noexcept;
        void removeContainedWord() noexcept;
        void listFauxElements() noexcept;

        bool addElement(const std::string& element) noexcept;
        void removedElement(const std::string& element, bool expected) noexcept override;
    private:
        // #region UserSet private members override 
        const Menu<UserSet, void>& setSpecificMenu() const noexcept override;
        // #endregion 

        std::set<std::string> fauxElements;

        friend class WordSet;
};