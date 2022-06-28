/*
    global-set.hpp

    GlobalSet is a set that consists of all elements that exist.
    It cannot be created as a subset of any set due to having the requirement of encompassing all elements.
*/
#pragma once

#include "user-set.hpp"

class GlobalSet : public UserSet {
    public:
        std::string_view name() const override;

        // #region UserSet public members override
        static constexpr char type_ = 'G';
        char type() const override { return type_; }

        void saveMachineSubset(std::ostream& saveLocation) const override;
        void loadMachineSubset(std::istream& loadLocation) override;

        const std::set<std::string>* elements() const override;
        const std::set<std::string>* complementElements() const override;
        // #endregion
    private:
        const Menu<void, std::unique_ptr<UserSet>, UserSet&, const std::string&>& createableSubsetMenu() const override;
};