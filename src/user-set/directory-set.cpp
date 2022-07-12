/*
    directory-set.cpp

    DirectorySet is a type of subset that contains and mirrors the file names that are within a directory

    Due to the subset mirroring a list of arbitrary words that would only coincidentally appear in other UserSets,
    the only type of UserSet that a DirectorySet can have as a parent is the global set
*/
#include "directory-set.hpp"

#include "helpers.hpp"

#include <filesystem>
#include <locale>

DirectorySet::DirectorySet(UserSet* parent, const pstring& name) noexcept
    : SubSet(parent, name, std::make_unique<std::set<pstring>>())
{}

DirectorySet::DirectorySet(UserSet* parent, const pstring& name, const std::filesystem::path& directory) noexcept
    : SubSet(parent, name, std::make_unique<std::set<pstring>>()), directory_(std::filesystem::absolute(directory))
{
    updateElements();
}

UserSet* DirectorySet::createSet(UserSet& parent, const pstring& name) noexcept {
    pcout << "Enter the name of the directory you would like this set to mirror: ";
    std::filesystem::path directory_;
    pCinIgnoreAll();;
    pCinGetLine(directory_);

    return new DirectorySet(&parent, name, directory_);
}

const auto DIRECTORY_SET_MENU = ReinterpretMenu<DirectorySet, UserSet, void>({
    {pliteral("LD"), {pliteral("List mirrored directory"), &DirectorySet::listMirroredDirectory}},
    {pliteral("X"), {pliteral("Exit set-specific options"), &DirectorySet::exitSetSpecificOptions}},
    {pstring(UserSet::EXIT_KEYWORD), {pliteral("Exit the program"), &DirectorySet::exitProgram}}
});

const Menu<UserSet, void>& DirectorySet::setSpecificMenu() const noexcept {
    return DIRECTORY_SET_MENU;
}

void DirectorySet::changeDirectory() noexcept {
    pcout << "Enter the name of the directory you would like this set to mirror: ";
    std::filesystem::path directory;
    pCinIgnoreAll();;
    pCinGetLine(directory);
    directory_ = std::filesystem::absolute(directory);

    updateElements();
}

void DirectorySet::listMirroredDirectory() noexcept {
    pcout << directory() << '\n';
}

pstring_view DirectorySet::directory() const noexcept {
    return directory_.c_str();
}

void DirectorySet::handleDirectoryError() noexcept {
    pcout << "The directory '" << directory() << "' is unaccessible,\n"
              << "you may either delete this set, change  exit the program, or the program can continue running with the previously gathered directory contents (this will be nothing on load)\n"
              << "Enter [D] to delete, [E] to exit the program without saving, or anything else to continue: ";
    pstring input;
    pcin >> input;
    if (ptoupper(input[0]) == 'D') {
        parent()->onQueryRemove = this;
        queryable = false;
        setSpecificQueryable = false;
    }
    if (ptoupper(input[0]) == 'E') {
        exit(0);
    }
}

void DirectorySet::saveMachineSubset(std::ostream& saveLocation) noexcept {
    pstring_view directoryString = directory_.c_str();
    std::string genericDirectoryString = genericStringFromPString(directoryString);
    saveLocation << genericDirectoryString.size() << ' ' << genericDirectoryString;
}

void DirectorySet::loadMachineSubset(std::istream& loadLocation) noexcept {
    size_t directorySize;
    loadLocation >> directorySize;
    
    std::string genericDirectoryString = std::string(directorySize + 1, '\0');
    skipRead(loadLocation, 1);
    loadLocation.read(genericDirectoryString.data(), directorySize);
    pstring directoryString = genericStringToPString(genericDirectoryString);

    directory_ = std::filesystem::absolute(directoryString);
}

void DirectorySet::updateElements() noexcept {
    std::set<pstring> newElements;
    try {
        if (!std::filesystem::is_directory(directory_)) {
            throw std::logic_error("Unresolveable, Directory set created on directory that does not exist.");
        }
        for (const auto& entry : std::filesystem::directory_iterator(directory_)) {
            newElements.insert(entry.path().lexically_relative(directory_).c_str());
        }
    } catch (...) {
        handleDirectoryError();
        return;
    }
    for (const auto& element : *elements_) {
        if (newElements.find(element) == newElements.end()) {
            removedElement(element, false);
        }
    }

    *elements_ = std::move(newElements);
}
