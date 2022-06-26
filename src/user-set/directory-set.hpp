#include "subset.hpp"

#include <filesystem>

class DirectorySet : public SubSet {
    public:
        DirectorySet(UserSet* parent, const std::string& name);
        DirectorySet(UserSet* parent, const std::string& name, const std::string& directory);

        static std::unique_ptr<UserSet> createSet(UserSet& parent, const std::string& name);

        static constexpr char setType_() { return 'D'; }
        char setType() const override { return setType_(); }

        void saveMachineSubset(std::ostream& saveLocation) const override;
        void loadMachineSubset(std::istream& loadLocation) override;

        bool listMirroredDirectory();

        const std::set<std::string>* elements() const override;
        const std::set<std::string>* complementElements() const override;
        std::string_view directory() const;
    private:
        const Menu<UserSet, bool>& setSpecificMenu() const override;

        bool isMirroring = false;
        // will always return mirroring the directory, is not logically a const-important member of this class
        mutable std::set<std::string> elements_;
        std::filesystem::path directory_;
}; 
