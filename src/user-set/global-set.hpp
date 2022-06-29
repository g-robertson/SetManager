/*
    global-set.hpp

    GlobalSet is a set that consists of all elements that exist.
    It cannot be created as a subset of any set due to having the requirement of encompassing all elements.
*/
#pragma once

#include "user-set.hpp"

class GlobalSet : public UserSet {
    public:
        std::string_view name() const noexcept override;

        // #region UserSet public members override
        static constexpr char type_ = 'G';
        char type() const noexcept override { return type_; }

        void saveMachineSubset(std::ostream& saveLocation) noexcept override;
        void loadMachineSubset(std::istream& loadLocation) noexcept override;

        const std::set<std::string>* elements() const noexcept override;
        const std::set<std::string>* complementElements() const noexcept override;
        // #endregion
    private:
        const Menu<void, UserSet*, UserSet&, const std::string&>& createableSubsetMenu() const noexcept override;
};