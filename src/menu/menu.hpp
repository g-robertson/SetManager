#pragma once

#include <string>
#include <list>
#include <map>
#include <locale>
#include <iostream>
#include <iomanip>


template <typename TClass, typename TReturn, typename... TArgs>
class Menu {
    public:
        virtual TReturn query(TClass& object, TArgs&... args) const = 0;
};

template <typename TReturn, typename... TArgs>
class Menu<void, TReturn, TArgs...> {
    public:
        virtual TReturn query(TArgs&... args) const = 0;
};

template <typename TClass, typename TReturn, typename... TArgs>
class StaticMenu : public Menu<TClass, TReturn, TArgs...> {
    public:
        using optionType = std::pair<std::string, TReturn (TClass::*)(TArgs...)>;
        StaticMenu(const std::list<std::pair<std::string, optionType>>& optionList, TReturn throwResult)
            : Menu<TClass, TReturn, TArgs...>(), throwResult(throwResult)
        {
            for (const auto& optionKVP : optionList) {
                std::string optionName = optionKVP.first;
                for (auto& character : optionName) {
                    character = std::toupper(character, std::locale());
                }

                auto mapIterator = optionMap_.insert({optionName, optionKVP.second});
                optionList_.push_back(mapIterator.first);
            }
        }

        TReturn query(TClass& object, TArgs&... args) const override {
            while (true) {
                std::cout << "Select an option (case-insensitive):\n"
                          << std::string(80, '-') << '\n';
                for (const auto& menuOptionKVP : optionList_) {
                    std::cout << '[' << menuOptionKVP->first << "]: " << menuOptionKVP->second.first << '\n';
                }
                std::cout << std::string(80, '-') << '\n';

                std::string input;
                std::cin >> input;
                for (auto& character : input) {
                    character = std::toupper(character, std::locale());
                }

                const auto& selectedMenuOptionKVP = optionMap_.find(input);
                if (selectedMenuOptionKVP != optionMap_.end()) {
                    try {
                        return (object.*(selectedMenuOptionKVP->second.second))(std::forward<TArgs>(args)...);
                    } catch (...) {
                        return throwResult;
                    }
                }
            }
        }
    
    private:
        std::map<std::string, optionType> optionMap_;
        std::list<typename decltype(optionMap_)::iterator> optionList_;
        TReturn throwResult;
};

template <typename TReturn, typename... TArgs>
class StaticMenu<void, TReturn, TArgs...> : public Menu<void, TReturn, TArgs...> {
    public:
        using optionType = std::pair<std::string, TReturn (*)(TArgs...)>;
        StaticMenu(const std::list<std::pair<std::string, optionType>>& optionList, TReturn throwResult)
            : Menu<void, TReturn, TArgs...>(), throwResult(throwResult)
        {
            for (const auto& optionKVP : optionList) {
                std::string optionName = optionKVP.first;
                for (auto& character : optionName) {
                    character = std::toupper(character, std::locale());
                }

                auto mapIterator = optionMap_.insert({optionName, optionKVP.second});
                optionList_.push_back(mapIterator.first);
            }
        }

        TReturn query(TArgs&... args) const override {
            while (true) {
                std::cout << "Select an option (case-insensitive):\n"
                          << std::string(80, '-') << '\n';
                for (const auto& menuOptionKVP : optionList_) {
                    std::cout << '[' << menuOptionKVP->first << "]: " << menuOptionKVP->second.first << '\n';
                }
                std::cout << std::string(80, '-') << '\n';

                std::string input;
                std::cin >> input;
                for (auto& character : input) {
                    character = std::toupper(character, std::locale());
                }

                const auto& selectedMenuOptionKVP = optionMap_.find(input);
                if (selectedMenuOptionKVP != optionMap_.end()) {
                    try {
                        return selectedMenuOptionKVP->second.second(std::forward<TArgs>(args)...);
                    } catch (...) {
                        return throwResult;
                    }
                }
            }
        }
    private:
        std::map<std::string, optionType> optionMap_;
        std::list<typename decltype(optionMap_)::iterator> optionList_;
        TReturn throwResult;
};

template <typename TDerived, typename TClass, typename TReturn, typename... TArgs>
class ReinterpretMenu : public Menu<TClass, TReturn, TArgs...> {
    public:
        using optionType = std::pair<std::string, TReturn (TDerived::*)(TArgs...)>;
        ReinterpretMenu(const std::list<std::pair<std::string, optionType>>& optionList, TReturn throwResult)
            : Menu<TClass, TReturn, TArgs...>(), throwResult(throwResult)
        {
            for (const auto& optionKVP : optionList) {
                std::string optionName = optionKVP.first;
                for (auto& character : optionName) {
                    character = std::toupper(character, std::locale());
                }

                auto mapIterator = optionMap_.insert({optionName, optionKVP.second});
                optionList_.push_back(mapIterator.first);
            }
        }

        TReturn query(TClass& object, TArgs&... args) const override {
            while (true) {
                std::cout << "Select an option (case-insensitive):\n"
                          << std::string(80, '-') << '\n';
                for (const auto& menuOptionKVP : optionList_) {
                    std::cout << '[' << menuOptionKVP->first << "]: " << menuOptionKVP->second.first << '\n';
                }
                std::cout << std::string(80, '-') << '\n';

                std::string input;
                std::cin >> input;
                for (auto& character : input) {
                    character = std::toupper(character, std::locale());
                }

                const auto& selectedMenuOptionKVP = optionMap_.find(input);
                if (selectedMenuOptionKVP != optionMap_.end()) {
                    try {
                        return (reinterpret_cast<TDerived&>(object).*(selectedMenuOptionKVP->second.second))(std::forward<TArgs>(args)...);
                    } catch (...) {
                        return throwResult;
                    }
                }
            }
        }
    private:
        std::map<std::string, optionType> optionMap_;
        std::list<typename decltype(optionMap_)::iterator> optionList_;
        TReturn throwResult;
};