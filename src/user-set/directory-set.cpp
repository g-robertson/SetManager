/*
    directory-set.cpp

    DirectorySet is a type of subset that contains and mirrors the file names that are within a directory

    Due to the subset mirroring a list of arbitrary words that would only coincidentally appear in other UserSets,
    the only type of UserSet that a DirectorySet can have as a parent is the global set
*/
#include "directory-set.hpp"

#include "helpers.hpp"

#include <filesystem>

DirectorySet::DirectorySet(UserSet* parent, const std::string& name)
    : SubSet(parent, name), isMirroring(false)
{}

DirectorySet::DirectorySet(UserSet* parent, const std::string& name, const std::string& directory)
    : SubSet(parent, name), isMirroring(true), directory_(std::filesystem::absolute(directory))
{}

UserSet* DirectorySet::createSet(UserSet& parent, const std::string& name) {
    std::cout << "Enter the name of the directory you would like this set to mirror: ";
    std::string directory;
    ignoreAll(std::cin);
    std::getline(std::cin, directory);

    auto* directorySet = new DirectorySet(&parent, name, directory);
    try {
        directorySet->updateDirectory();
        return directorySet;
    // Don't really care what exception happened, it is likely a logic_error (I threw it), or a filesystem_error (The directory iterator threw it)
    // The only real problem is that the directory set can't be created in a state where directory access fails or throws, so return nothing
    } catch (const std::exception&) {
        std::cout << "Could not create a directory set because there is no directory with the name '" << directory << "'.\n";
        delete directorySet;
        return nullptr;
    }
}

const auto DIRECTORY_SET_MENU = ReinterpretMenu<DirectorySet, UserSet, bool>({
    {"LD", {"List mirrored directory", &DirectorySet::listMirroredDirectory}},
    {"U", {"Update mirrored directory contents", &DirectorySet::updateDirectory}},
    {"X", {"Exit set-specific options", &DirectorySet::moveUpHierarchy}},
    {UserSet::EXIT_KEYWORD, {"Exit the program", &DirectorySet::exitProgram}}
}, true);

const Menu<UserSet, bool>& DirectorySet::setSpecificMenu() const {
    return DIRECTORY_SET_MENU;
}

bool DirectorySet::updateDirectory() {
    if (!std::filesystem::is_directory(directory_)) {
        throw std::logic_error("Unresolveable, Directory set created on directory that does not exist.");
    }
    std::set<std::string> newElements;
    for (const auto& entry : std::filesystem::directory_iterator(directory_)) {
        newElements.insert(entry.path().lexically_relative(directory_).string());
    }

    for (const auto& element : elements_) {
        if (newElements.find(element) == newElements.end()) {
            removedElement(element, false);
        }
    }

    elements_ = std::move(newElements);
    return true;
}

bool DirectorySet::listMirroredDirectory() {
    std::cout << directory() << '\n';
    return true;
}

const std::set<std::string>* DirectorySet::elements() const {
    if (!isMirroring) {
        throw std::logic_error("Unappearable [Attempted to interact with directory set that is not mirroring]");
    }

    return &elements_;
}

const std::set<std::string>* DirectorySet::complementElements() const {
    if (!isMirroring) {
        throw std::logic_error("Unappearable [Attempted to interact with directory set that is not mirroring]");
    }

    return nullptr;
}

std::string_view DirectorySet::directory() const {
    if (!isMirroring) {
        throw std::logic_error("Unappearable [Attempted to interact with directory set that is not mirroring]");
    }

    return directory_.c_str();
}

void DirectorySet::saveMachineSubset(std::ostream& saveLocation) const {
    if (!isMirroring) {
        throw std::logic_error("Unappearable [Attempted to interact with directory set that is not mirroring]");
    }

    std::string_view directoryString = directory_.c_str();
    saveLocation << directoryString.size() << ' ' << directoryString;
}

void DirectorySet::loadMachineSubset(std::istream& loadLocation) {
    size_t directorySize;
    loadLocation >> directorySize;
    // directory names require null termination, I don't know why, it's probably documented somewhere
    std::string directoryString = std::string('0', directorySize + 1);
    directoryString[directorySize] = '\0';
    skipRead(loadLocation, 1);
    loadLocation.read(directoryString.data(), directorySize);

    directory_ = std::filesystem::absolute(directoryString);
    isMirroring = true;
    if (!std::filesystem::exists(directory_)) {
        throw std::logic_error("Load-resolveable, [Directory mirrored by directory set did not exist]");
    }
    if (!std::filesystem::is_directory(directory_)) {
        throw std::logic_error("Load-resolveable, [Directory mirrored by directory set is not a directory]");
    }
    updateDirectory();
}