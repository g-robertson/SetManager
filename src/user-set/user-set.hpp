/*
    user-set.hpp

    UserSet is the most abstract type of set, containing all methods that are possible for all set types to have
    It serves as the interface that all sets can interact with eachother with
*/
#pragma once

#include "menu.hpp"

#include <string>
#include <set>
#include <memory>

class UserSet {
    public:
        UserSet() noexcept = default;
        UserSet(UserSet* parent) noexcept;
        virtual ~UserSet() noexcept = default;

        virtual std::string_view name() const noexcept = 0;
        virtual bool preQuery() noexcept;
        virtual void postParentLoad() noexcept(false);
        bool query() noexcept;
        UserSet* queryForSubset() noexcept;
        UserSet* selectForSubset() noexcept;
        UserSet* enterForSubset() noexcept;
        UserSet* exitSubsetMenu() noexcept;

        void setSpecificOptions() noexcept;
        void exitSetSpecificOptions() noexcept;
        void saveHumanAllConnectedSubsets() noexcept;
        void saveMachineAllConnectedSubsets() noexcept;
        void loadMachineAllConnectedSubsets() noexcept;
        void listSubsets() noexcept;
        void listElements() noexcept;
        void createSubset() noexcept;
        void deleteSubset() noexcept;
        void enterSubset() noexcept;
        void moveUpHierarchy() noexcept;
        void exitProgram() noexcept;

        virtual void saveMachineSubset(std::ostream& saveLocation) noexcept = 0;
        void saveMachineSubsets(std::ostream& saveLocation) noexcept;
        void saveHumanSubsets(std::ostream& saveLocation) noexcept;
        virtual void loadMachineSubset(std::istream& loadLocation) noexcept = 0;
        void loadMachineSubsets(std::istream& loadLocation) noexcept;

        virtual char type() const noexcept = 0;

        bool contains(const std::string& element) const noexcept;
        virtual void removedElement(const std::string& element, bool expected) noexcept;
        virtual const std::set<std::string>* elements() const noexcept = 0;
        virtual const std::set<std::string>* complementElements() const noexcept = 0;

        const static std::set<std::string> NO_ELEMENTS;
        const static std::string EXIT_KEYWORD;
        const static std::string DEFAULT_MACHINE_LOCATION;
        const static std::string DEFAULT_HUMAN_LOCATION;

        static UserSet* EXIT_SET_MENU(UserSet&, const std::string&) noexcept;

        const UserSet* parent() const noexcept;
        UserSet* parent() noexcept;
        const std::map<std::string, std::unique_ptr<UserSet>>& subsets() const noexcept;

        UserSet* onQueryRemove = nullptr;
        std::unique_ptr<UserSet> onQueryAdd;
        UserSet* onQueryEnter = nullptr;
    protected:
        bool queryable = false;
        bool setSpecificQueryable = false;
        UserSet* parent_;
        std::map<std::string, std::unique_ptr<UserSet>> subsets_;

    private:
        const Menu<UserSet, void>& menu() const noexcept;
        virtual const Menu<void, UserSet*, UserSet&, const std::string&>& createableSubsetMenu() const noexcept = 0;
        virtual const Menu<UserSet, void>& setSpecificMenu() const noexcept;

        void saveAllConnectedSubsets(void (UserSet::*saveMethod)(std::ostream& saveLocation), std::string_view defaultSaveLocation) noexcept;
        void loadAllConnectedSubsets(void (UserSet::*loadMethod)(std::istream& loadLocation), std::string_view defaultLoadLocation) noexcept;
        void saveSubsets(void (UserSet::*saveMethod)(std::ostream& saveLocation), std::ofstream& saveLocation) const noexcept;
        void loadSubsets(void (UserSet::*loadMethod)(std::istream& loadLocation), std::ifstream& loadLocation) noexcept;

        bool loadFailed = false;

        void saveHumanSubsets_(std::ostream& saveLocation, int indentation) noexcept;
        void loadMachineSubsets_(std::istream& loadLocation) noexcept(false);

        void onQuery() noexcept;
};