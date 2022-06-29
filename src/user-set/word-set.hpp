/*
    word-set.hpp

    WordSet is a type of subset that can contain any element that exists in the parent UserSet by selection, or entering the word.

    Due to its possibilities of being capable of dealing with all words arbitrarily in any parent set, it can have any UserSet type as its parent 
*/
#pragma once

#include "subset.hpp"

class WordSet : public SubSet {
    public:
        WordSet(UserSet* parent, const std::string& name) noexcept;

        // #region SubSet public members override 
        static UserSet* createSet(UserSet& parent, const std::string& name) noexcept;
        // #endregion 
        // #region UserSet public members override 
        static constexpr char type_ = 'W';
        char type() const noexcept override { return type_; }

        void saveMachineSubset(std::ostream& saveLocation) noexcept override;
        void loadMachineSubset(std::istream& loadLocation) noexcept override;

        const std::set<std::string>* elements() const noexcept override;
        const std::set<std::string>* complementElements() const noexcept override;
        // #endregion 

        void addWord() noexcept;
        void addParentWord() noexcept;
        void removeWord() noexcept;
        void removeContainedWord() noexcept;

        void removedElement(const std::string& element, bool expected) noexcept override;
    private:
        // #region UserSet private members override 
        const Menu<UserSet, void>& setSpecificMenu() const noexcept override;
        // #endregion 

        void handleUnexpectedWordRemoval(const std::string& element) noexcept;

        std::set<std::string> elements_;

        bool isBecomingFaux = false;
        friend class FauxWordSet;
}; 
