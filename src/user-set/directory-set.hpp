/*
    directory-set.hpp

    DirectorySet is a type of subset that contains and mirrors the file names that are within a directory

    Due to the subset mirroring a list of arbitrary words that would only coincidentally appear in other UserSets,
    the only type of UserSet that a DirectorySet can have as a parent is the global set
*/
#pragma once

#include "subset.hpp"

#include <filesystem>

class DirectorySet : public SubSet {
    public:
        DirectorySet(UserSet* parent, const std::string& name);
        DirectorySet(UserSet* parent, const std::string& name, const std::string& directory);

        // #region SubSet public members override 
        static UserSet* createSet(UserSet& parent, const std::string& name);
        // #endregion 
        // #region UserSet public members override 
        static constexpr char type_ = 'D';
        char type() const override { return type_; }

        void saveMachineSubset(std::ostream& saveLocation) const override;
        void loadMachineSubset(std::istream& loadLocation) override;

        const std::set<std::string>* elements() const override;
        const std::set<std::string>* complementElements() const override;
        // #endregion 

        bool updateDirectory();
        bool listMirroredDirectory();

        std::string_view directory() const;
    private:
        // #region UserSet private members override 
        const Menu<UserSet, bool>& setSpecificMenu() const override;
        // #endregion 

        bool isMirroring = false;
        // will always return mirroring the directory, is not logically a const-important member of this class
        mutable std::set<std::string> elements_;
        std::filesystem::path directory_;
}; 
