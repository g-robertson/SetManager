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
        DirectorySet(UserSet* parent, const std::string& name) noexcept;
        DirectorySet(UserSet* parent, const std::string& name, const std::string& directory) noexcept;

        // #region SubSet public members override 
        static UserSet* createSet(UserSet& parent, const std::string& name) noexcept;
        // #endregion 
        // #region UserSet public members override 
        static constexpr char type_ = 'D';
        char type() const noexcept override { return type_; }

        void saveMachineSubset(std::ostream& saveLocation) const noexcept override;
        void loadMachineSubset(std::istream& loadLocation) noexcept override;

        const std::set<std::string>* elements() const noexcept override;
        const std::set<std::string>* complementElements() const noexcept override;
        // #endregion 

        void changeDirectory() noexcept;
        void updateDirectory() noexcept;
        void listMirroredDirectory() noexcept;

        std::string_view directory() const noexcept;
    private:
        // #region UserSet private members override 
        const Menu<UserSet, void>& setSpecificMenu() const noexcept override;
        // #endregion 

        void handleDirectoryError() noexcept;

        // will always return mirroring the directory, is not logically a const-important member of this class
        mutable std::set<std::string> elements_;
        std::filesystem::path directory_;
}; 
