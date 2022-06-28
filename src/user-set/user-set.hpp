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
        UserSet() = default;
        UserSet(UserSet* parent);
        virtual ~UserSet() = default;

        virtual std::string_view name() const = 0;
        bool query();

        void setSpecificOptions();
        void exitSetSpecificOptions();
        void saveHumanAllConnectedSubsets();
        void saveMachineAllConnectedSubsets();
        void loadMachineAllConnectedSubsets();
        void listSubsets();
        void listElements();
        void createSubset();
        void deleteSubset();
        void enterSubset();
        void moveUpHierarchy();
        void exitProgram();

        virtual void saveMachineSubset(std::ostream& saveLocation) const = 0;
        void saveMachineSubsets(std::ostream& saveLocation) const;
        void saveHumanSubsets(std::ostream& saveLocation) const;
        virtual void loadMachineSubset(std::istream& loadLocation) = 0;
        void loadMachineSubsets(std::istream& loadLocation);

        virtual char type() const = 0;

        bool contains(const std::string& element) const;
        virtual void removedElement(const std::string& element, bool expected);
        virtual const std::set<std::string>* elements() const = 0;
        virtual const std::set<std::string>* complementElements() const = 0;

        const static std::set<std::string> NO_ELEMENTS;
        const static std::string EXIT_KEYWORD;
        const static std::string DEFAULT_MACHINE_LOCATION;
        const static std::string DEFAULT_HUMAN_LOCATION;
        static UserSet* EXIT_SET_MENU(UserSet&, const std::string&);

        UserSet* onQueryRemove = nullptr;
    protected:
        bool queryable = false;
        bool setSpecificQueryable = false;
        UserSet* parent;
        std::map<std::string, std::unique_ptr<UserSet>> subsets;

    private:
        const Menu<UserSet, void>& menu() const;
        virtual const Menu<void, UserSet*, UserSet&, const std::string&>& createableSubsetMenu() const = 0;
        virtual const Menu<UserSet, void>& setSpecificMenu() const;

        void saveAllConnectedSubsets(void (UserSet::*saveMethod)(std::ostream& saveLocation) const, std::string_view defaultSaveLocation) const;
        void loadAllConnectedSubsets(void (UserSet::*loadMethod)(std::istream& loadLocation), std::string_view defaultLoadLocation);
        void saveSubsets(void (UserSet::*saveMethod)(std::ostream& saveLocation), std::ofstream& saveLocation) const;
        void loadSubsets(void (UserSet::*loadMethod)(std::istream& loadLocation), std::ifstream& loadLocation);

        void saveHumanSubsets_(std::ostream& saveLocation, int indentation) const;
        void loadMachineSubsets_(std::istream& loadLocation);
};