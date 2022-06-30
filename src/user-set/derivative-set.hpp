/*
    derivative-set.hpp

    DerivativeSet is a set that holds the properties of a subset, while also requiring elements of certain subsets of its parent set
    It is an abstract type of set due to not having any defined methods for gathering elements, complement elements, or whom it derives from
    Its purpose is to provide an interface for ensuring that all subsets necessary for a derivative subset are available before finishing its load

    Note: It is undefined behavior to instantiate this class with the parent, name constructor and then not run postParentLoad() hook
*/
#pragma once

#include "subset.hpp"

#include <memory>
#include <vector>
#include <set>

class DerivativeSet : public SubSet {
    public:
        DerivativeSet(UserSet* parent, const std::string& name, std::initializer_list<UserSet*>) noexcept;
        // Note: It is undefined behavior to instantiate this class with the parent, name constructor and then not run postParentLoad() hook
        DerivativeSet(UserSet* parent, const std::string& name) noexcept;
        virtual ~DerivativeSet() noexcept;

        const std::vector<UserSet*>& derivesFrom() const noexcept;

        void saveMachineSubset(std::ostream& saveLocation) noexcept override;
        void loadMachineSubset(std::istream& saveLocation) noexcept override;
        virtual void saveMachineDerivativeSubset(std::ostream& saveLocation) noexcept = 0;
        virtual void loadMachineDerivativeSubset(std::istream& loadLocation) noexcept = 0;

        bool postParentLoad() noexcept override;
        virtual bool postPostParentLoad() noexcept;
    protected:
        std::vector<UserSet*>& derivesFrom_() noexcept;
    private:
        union {
            std::vector<UserSet*>* derivesFrom__;
            std::vector<std::string>* derivesFromNames__;
        };
        bool postParentLoading = false;
        bool postParentLoaded = false;
}; 