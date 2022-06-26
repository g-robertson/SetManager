#pragma once

#include "user-set.hpp"

class SubSet : public UserSet {
    public:
        SubSet(UserSet* parent, const std::string& name);

        std::string_view setName() const override;
    private:
        const Menu<void, std::unique_ptr<UserSet>, UserSet&, const std::string&>& createableSubsetMenu() const override;

        std::string name_;
};