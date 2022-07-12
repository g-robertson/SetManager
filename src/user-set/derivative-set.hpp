/*
    derivative-set.hpp

    DerivativeSet is a set that holds the properties of a subset, while also requiring elements of certain subsets of its parent set
    It is an abstract type of set due to not having any defined methods for gathering elements, complement elements, or whom it derives from
    Its purpose is to provide an interface for ensuring that all subsets necessary for a derivative subset are available before finishing its load

    Note: It is undefined behavior to instantiate this class with the parent, name constructor and then not run postSiblingsLoad() hook
*/
#pragma once

#include "subset.hpp"

#include <memory>
#include <vector>
#include <set>

class DerivativeSet : public SubSet {
    public:
        DerivativeSet(
            UserSet* parent,
            const pstring& name,
            std::initializer_list<UserSet*> userSets,
            std::unique_ptr<std::set<pstring>> elements = std::unique_ptr<std::set<pstring>>(),
            std::unique_ptr<std::set<pstring>> complementElements = std::unique_ptr<std::set<pstring>>()
        ) noexcept;
        // Note: It is undefined behavior to instantiate this class with the parent, name constructor and then not run postSiblingsLoad() hook
        DerivativeSet(
            UserSet* parent,
            const pstring& name,
            std::unique_ptr<std::set<pstring>> elements = std::unique_ptr<std::set<pstring>>(),
            std::unique_ptr<std::set<pstring>> complementElements = std::unique_ptr<std::set<pstring>>()
        ) noexcept;
        virtual ~DerivativeSet() noexcept;

        const std::vector<UserSet*>& derivesFrom() const noexcept;

        void saveMachineSubset(std::ostream& saveLocation) noexcept override;
        void loadMachineSubset(std::istream& saveLocation) noexcept override;
        virtual void saveMachineDerivativeSubset(std::ostream& saveLocation) noexcept;
        virtual void loadMachineDerivativeSubset(std::istream& loadLocation) noexcept;

        void postSiblingsLoad() noexcept(false) override;
        virtual void postPostSiblingsLoad() noexcept(false);
    protected:
        std::vector<UserSet*>& derivesFrom() noexcept;
    private:
        union {
            std::vector<UserSet*>* derivesFrom_;
            std::vector<std::vector<pstring>>* derivesFromNames_;
        };
        bool postSiblingsLoading = false;
        bool postSiblingsLoaded = false;
}; 