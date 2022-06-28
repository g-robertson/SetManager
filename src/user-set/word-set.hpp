/*
    word-set.hpp

    WordSet is a type of subset that can contain any element that exists in the parent UserSet by selection, or entering the word.

    Due to its possibilities of being capable of dealing with all words arbitrarily in any parent set, it can have any UserSet type as its parent 
*/
#pragma once

#include "subset.hpp"

class WordSet : public SubSet {
    public:
        WordSet(UserSet* parent, const std::string& name);

        // #region SubSet public members override 
        static UserSet* createSet(UserSet& parent, const std::string& name);
        // #endregion 
        // #region UserSet public members override 
        static constexpr char type_ = 'W';
        char type() const override { return type_; }

        void saveMachineSubset(std::ostream& saveLocation) const override;
        void loadMachineSubset(std::istream& loadLocation) override;

        const std::set<std::string>* elements() const override;
        const std::set<std::string>* complementElements() const override;
        // #endregion 

        bool addWord();
        bool addParentWord();
        bool removeWord();
        bool removeContainedWord();

        void removedElement(const std::string& element, bool expected) override;
    private:
        // #region UserSet private members override 
        const Menu<UserSet, bool>& setSpecificMenu() const override;
        // #endregion 

        std::set<std::string> elements_;
}; 
