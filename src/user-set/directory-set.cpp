/*
    directory-set.cpp

    DirectorySet is a type of subset that contains and mirrors the file names that are within a directory

    Due to the subset mirroring a list of arbitrary words that would only coincidentally appear in other UserSets,
    the only type of UserSet that a DirectorySet can have as a parent is the global set
*/
#include "directory-set.hpp"

#include "helpers.hpp"

#include <filesystem>

DirectorySet::DirectorySet(UserSet* parent, const std::string& name) noexcept
    : SubSet(parent, name)
{}

DirectorySet::DirectorySet(UserSet* parent, const std::string& name, const std::string& directory) noexcept
    : SubSet(parent, name), directory_(std::filesystem::absolute(directory))
{
    updateDirectory();
}

UserSet* DirectorySet::createSet(UserSet& parent, const std::string& name) noexcept {
    std::cout << "Enter the name of the directory you would like this set to mirror: ";
    std::string directory;
    ignoreAll(std::cin);
    std::getline(std::cin, directory);

    return new DirectorySet(&parent, name, directory);
}

const auto DIRECTORY_SET_MENU = ReinterpretMenu<DirectorySet, UserSet, void>({
    {"LD", {"List mirrored directory", &DirectorySet::listMirroredDirectory}},
    {"U", {"Update mirrored directory contents", &DirectorySet::updateDirectory}},
    {"X", {"Exit set-specific options", &DirectorySet::exitSetSpecificOptions}},
    {UserSet::EXIT_KEYWORD, {"Exit the program", &DirectorySet::exitProgram}}
});

const Menu<UserSet, void>& DirectorySet::setSpecificMenu() const noexcept {
    return DIRECTORY_SET_MENU;
}

void DirectorySet::changeDirectory() noexcept {
    std::cout << "Enter the name of the directory you would like this set to mirror: ";
    std::string directory;
    ignoreAll(std::cin);
    std::getline(std::cin, directory);
    directory_ = std::filesystem::absolute(directory);

    updateDirectory();
}

void DirectorySet::updateDirectory() noexcept {
    std::set<std::string> newElements;
    try {
        if (!std::filesystem::is_directory(directory_)) {
            throw std::logic_error("Unresolveable, Directory set created on directory that does not exist.");
        }
        for (const auto& entry : std::filesystem::directory_iterator(directory_)) {
            newElements.insert(entry.path().lexically_relative(directory_).string());
        }
    } catch (...) {
        handleDirectoryError();
        return;
    }
    for (const auto& element : elements_) {
        if (newElements.find(element) == newElements.end()) {
            removedElement(element, false);
        }
    }

    elements_ = std::move(newElements);
}

void DirectorySet::listMirroredDirectory() noexcept {
    std::cout << directory() << '\n';
}

const std::set<std::string>* DirectorySet::elements() const noexcept {
    return &elements_;
}

const std::set<std::string>* DirectorySet::complementElements() const noexcept {
    return nullptr;
}

std::string_view DirectorySet::directory() const noexcept {
    return directory_.c_str();
}

void DirectorySet::handleDirectoryError() noexcept {
    std::cout << "The directory '" << directory() << "' is unaccessible,\n"
              << "you may either delete this set, change  exit the program, or the program can continue running with the previously gathered directory contents (this will be nothing on load)\n"
              << "Enter [D] to delete, [E] to exit the program without saving, or anything else to continue: ";
    std::string input;
    std::cin >> input;
    if (std::toupper(input[0], std::locale()) == 'D') {
        parent->onQueryRemove = this;
        queryable = false;
        setSpecificQueryable = false;
    }
    if (std::toupper(input[0], std::locale()) == 'E') {
        exit(0);
    }
}

void DirectorySet::saveMachineSubset(std::ostream& saveLocation) noexcept {
    std::string_view directoryString = directory_.c_str();
    saveLocation << directoryString.size() << ' ' << directoryString;
}

void DirectorySet::loadMachineSubset(std::istream& loadLocation) noexcept {
    size_t directorySize;
    loadLocation >> directorySize;
    // directory names require null termination, I don't know why, it's probably documented somewhere
    std::string directoryString = std::string(directorySize + 1, '\0');
    skipRead(loadLocation, 1);
    loadLocation.read(directoryString.data(), directorySize);

    directory_ = std::filesystem::absolute(directoryString);
    updateDirectory();
}