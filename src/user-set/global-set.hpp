#pragma once

#include "user-set.hpp"

class GlobalSet : public UserSet {
    public:
        std::string_view setName() const override;
        static constexpr char setType_() { return 'G'; }
        char setType() const override { return setType_(); }

        void saveMachineSubset(std::ostream&) const override {}
        void loadMachineSubset(std::istream&) override {}

        const std::set<std::string>* elements() const override;
        const std::set<std::string>* complementElements() const override;
    private:
        const Menu<void, std::unique_ptr<UserSet>, UserSet&, const std::string&>& createableSubsetMenu() const override;
};