#include "subset.hpp"

class WordSet : public SubSet {
    public:
        WordSet(UserSet* parent, const std::string& name);

        static std::unique_ptr<UserSet> createSet(UserSet& parent, const std::string& name);

        static constexpr char setType_() { return 'W'; }
        char setType() const override { return setType_(); }

        bool addWord();
        bool addParentWord();
        bool removeWord();
        bool removeContainedWord();

        void removedElement(const std::string& element) override;

        void saveMachineSubset(std::ostream& saveLocation) const override;
        void loadMachineSubset(std::istream& loadLocation) override;

        const std::set<std::string>* elements() const override;
        const std::set<std::string>* complementElements() const override;
    private:
        const Menu<UserSet, bool>& setSpecificMenu() const override;

        std::set<std::string> elements_;
}; 
