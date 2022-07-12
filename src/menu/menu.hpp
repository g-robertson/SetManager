#pragma once

#include "platform.hpp"

#include <string>
#include <list>
#include <map>
#include <locale>
#include <iostream>
#include <iomanip>


template <typename TClass, typename TReturn, typename... TArgs>
class Menu {
    public:
        virtual TReturn query(TClass& object, TArgs&... args) const noexcept = 0;
};

template <typename TReturn, typename... TArgs>
class Menu<void, TReturn, TArgs...> {
    public:
        virtual TReturn query(TArgs&... args) const noexcept = 0;
};

template <typename TClass, typename TReturn, typename... TArgs>
class StaticMenu : public Menu<TClass, TReturn, TArgs...> {
    public:
        using optionType = std::pair<pstring, TReturn (TClass::*)(TArgs...) noexcept>;
        StaticMenu(const std::list<std::pair<pstring, optionType>>& optionList)
            : Menu<TClass, TReturn, TArgs...>()
        {
            for (const auto& optionKVP : optionList) {
                pstring optionName = optionKVP.first;
                for (auto& character : optionName) {
                    character = std::toupper(character, std::locale());
                }

                auto mapIterator = optionMap_.insert({optionName, optionKVP.second});
                optionList_.push_back(mapIterator.first);
            }
        }

        TReturn query(TClass& object, TArgs&... args) const noexcept override {
            while (true) {
                pcout << "Select an option (case-insensitive):\n"
                          << pstring(80, '-') << '\n';
                for (const auto& menuOptionKVP : optionList_) {
                    pcout << '[' << menuOptionKVP->first << "]: " << menuOptionKVP->second.first << '\n';
                }
                pcout << pstring(80, '-') << '\n';

                pstring input;
                pcin >> input;
                for (auto& character : input) {
                    character = std::toupper(character, std::locale());
                }

                const auto& selectedMenuOptionKVP = optionMap_.find(input);
                if (selectedMenuOptionKVP != optionMap_.end()) {
                    return (object.*(selectedMenuOptionKVP->second.second))(std::forward<TArgs>(args)...);
                }
            }
        }
    
    private:
        std::map<pstring, optionType> optionMap_;
        std::list<typename decltype(optionMap_)::iterator> optionList_;
};

template <typename TReturn, typename... TArgs>
class StaticMenu<void, TReturn, TArgs...> : public Menu<void, TReturn, TArgs...> {
    public:
        using optionType = std::pair<pstring, TReturn (*)(TArgs...) noexcept>;
        StaticMenu(const std::list<std::pair<pstring, optionType>>& optionList)
            : Menu<void, TReturn, TArgs...>()
        {
            for (const auto& optionKVP : optionList) {
                pstring optionName = optionKVP.first;
                for (auto& character : optionName) {
                    character = std::toupper(character, std::locale());
                }

                auto mapIterator = optionMap_.insert({optionName, optionKVP.second});
                optionList_.push_back(mapIterator.first);
            }
        }

        TReturn query(TArgs&... args) const noexcept override {
            while (true) {
                pcout << "Select an option (case-insensitive):\n"
                          << pstring(80, '-') << '\n';
                for (const auto& menuOptionKVP : optionList_) {
                    pcout << '[' << menuOptionKVP->first << "]: " << menuOptionKVP->second.first << '\n';
                }
                pcout << pstring(80, '-') << '\n';

                pstring input;
                pcin >> input;
                for (auto& character : input) {
                    character = std::toupper(character, std::locale());
                }

                const auto& selectedMenuOptionKVP = optionMap_.find(input);
                if (selectedMenuOptionKVP != optionMap_.end()) {
                    return selectedMenuOptionKVP->second.second(std::forward<TArgs>(args)...);
                }
            }
        }
    private:
        std::map<pstring, optionType> optionMap_;
        std::list<typename decltype(optionMap_)::iterator> optionList_;
};

template <typename TDerived, typename TClass, typename TReturn, typename... TArgs>
class ReinterpretMenu : public Menu<TClass, TReturn, TArgs...> {
    public:
        using optionType = std::pair<pstring, TReturn (TDerived::*)(TArgs...) noexcept>;
        ReinterpretMenu(const std::list<std::pair<pstring, optionType>>& optionList)
            : Menu<TClass, TReturn, TArgs...>()
        {
            for (const auto& optionKVP : optionList) {
                pstring optionName = optionKVP.first;
                for (auto& character : optionName) {
                    character = std::toupper(character, std::locale());
                }

                auto mapIterator = optionMap_.insert({optionName, optionKVP.second});
                optionList_.push_back(mapIterator.first);
            }
        }

        TReturn query(TClass& object, TArgs&... args) const noexcept override {
            while (true) {
                pcout << "Select an option (case-insensitive):\n"
                          << pstring(80, '-') << '\n';
                for (const auto& menuOptionKVP : optionList_) {
                    pcout << '[' << menuOptionKVP->first << "]: " << menuOptionKVP->second.first << '\n';
                }
                pcout << pstring(80, '-') << '\n';

                pstring input;
                pcin >> input;
                for (auto& character : input) {
                    character = std::toupper(character, std::locale());
                }

                const auto& selectedMenuOptionKVP = optionMap_.find(input);
                if (selectedMenuOptionKVP != optionMap_.end()) {
                    return (reinterpret_cast<TDerived&>(object).*(selectedMenuOptionKVP->second.second))(std::forward<TArgs>(args)...);
                }
            }
        }
    private:
        std::map<pstring, optionType> optionMap_;
        std::list<typename decltype(optionMap_)::iterator> optionList_;
};