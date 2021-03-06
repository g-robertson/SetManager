/*
    user-set.hpp

    UserSet is the most abstract type of set, containing all methods that are possible for all set types to have
    It serves as the interface that all sets can interact with eachother with
*/
#pragma once

#include "menu.hpp"
#include <string>
#include <set>
#include <filesystem>
#include <memory>

#include <nowide/fstream.hpp>

class UserSet {
    public:
        UserSet(
            std::unique_ptr<std::set<std::string>> elements = std::unique_ptr<std::set<std::string>>(),
            std::unique_ptr<std::set<std::string>> complementElements = std::unique_ptr<std::set<std::string>>()
        ) noexcept;
        UserSet(
            UserSet* parent,
            std::unique_ptr<std::set<std::string>> elements = std::unique_ptr<std::set<std::string>>(),
            std::unique_ptr<std::set<std::string>> complementElements = std::unique_ptr<std::set<std::string>>()
        ) noexcept;
        virtual ~UserSet() noexcept = default;

        virtual std::string_view name() const noexcept = 0;
        virtual bool preQuery() noexcept;
        void postParentLoad() noexcept(false);
        virtual void postSiblingsLoad() noexcept(false);
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
        void toggleHumanInclusion() noexcept;
        void toggleHumanInclusionRecursively() noexcept;
        void toggleHumanInclusionRecursively_(bool state) noexcept;
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
        void updateInternalElements() noexcept;
        virtual void updateElements() noexcept = 0;
        const std::set<std::string>* elements() const noexcept;
        const std::set<std::string>* complementElements() const noexcept;

        constexpr static std::string_view EXIT_KEYWORD = "EXIT";
        const static std::set<std::string> NO_ELEMENTS;
        const static std::filesystem::path DEFAULT_MACHINE_LOCATION;
        const static std::filesystem::path DEFAULT_HUMAN_LOCATION;

        static UserSet* EXIT_SET_MENU(UserSet&, const std::string&) noexcept;

        const UserSet* parent() const noexcept;
        UserSet* parent() noexcept;
        const std::map<std::string, std::unique_ptr<UserSet>>& subsets() const noexcept;

        UserSet* onQueryRemove = nullptr;
        std::unique_ptr<UserSet> onQueryAdd;
        UserSet* onQueryEnter = nullptr;
        bool humanIncluded = true;
    protected:
        bool queryable = false;
        bool setSpecificQueryable = false;
        UserSet* parent_;
        std::map<std::string, std::unique_ptr<UserSet>> subsets_;
        std::unique_ptr<std::set<std::string>> elements_;
        std::unique_ptr<std::set<std::string>> complementElements_;

    private:
        const Menu<UserSet, void>& menu() const noexcept;
        virtual const Menu<void, UserSet*, UserSet&, const std::string&>& createableSubsetMenu() const noexcept = 0;
        virtual const Menu<UserSet, void>& setSpecificMenu() const noexcept;

        void saveAllConnectedSubsets(void (UserSet::*saveMethod)(std::ostream& saveLocation), const std::filesystem::path& defaultSaveLocation) noexcept;
        void loadAllConnectedSubsets(void (UserSet::*loadMethod)(std::istream& loadLocation), const std::filesystem::path& defaultLoadLocation) noexcept;
        void saveSubsets(void (UserSet::*saveMethod)(std::ostream& saveLocation), nowide::ofstream& saveLocation) const noexcept;
        void loadSubsets(void (UserSet::*loadMethod)(std::istream& loadLocation), nowide::ifstream& loadLocation) noexcept;

        void saveHumanSubsets_(std::ostream& saveLocation, int indentation) noexcept;
        void loadMachineSubsets_(std::istream& loadLocation) noexcept(false);

        void onQuery() noexcept;
};