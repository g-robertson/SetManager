#include "directory-set.hpp"

#include "helpers.hpp"

#include <filesystem>

DirectorySet::DirectorySet(UserSet* parent, const std::string& name)
    : SubSet(parent, name), isMirroring(false)
{}

DirectorySet::DirectorySet(UserSet* parent, const std::string& name, const std::string& directory)
    : SubSet(parent, name), isMirroring(true), directory_(std::filesystem::absolute(directory))
{
    if (!std::filesystem::is_directory(directory_)) {
        throw std::logic_error("Directory set created on directory that does not exist.");
    }
}

std::unique_ptr<UserSet> DirectorySet::createSet(UserSet& parent, const std::string& name) {
    std::cout << "Enter the name of the directory you would like this set to mirror: ";
    std::string directory;
    ignoreAll(std::cin);
    std::getline(std::cin, directory);

    if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
        return std::unique_ptr<UserSet>(new DirectorySet(&parent, name, directory));
    }

    std::cout << "Could not create a directory set because there is no directory with the name '" << directory << "'.\n";
    return nullptr;
}

const auto DIRECTORY_SET_MENU = ReinterpretMenu<DirectorySet, UserSet, bool>({
    {"LD", {"List mirrored directory", &DirectorySet::listMirroredDirectory}},
    {"X", {"Exit set-specific options", &DirectorySet::moveUpHierarchy}},
    {UserSet::EXIT_KEYWORD, {"Exit the program", &DirectorySet::exitProgram}}
});

const Menu<UserSet, bool>& DirectorySet::setSpecificMenu() const {
    return DIRECTORY_SET_MENU;
}

bool DirectorySet::listMirroredDirectory() {
    std::cout << directory() << '\n';
    return true;
}

const std::set<std::string>* DirectorySet::elements() const {
    if (!isMirroring) {
        throw std::logic_error("Unresolveable [Attempted to interact with directory set that is not mirroring]");
    }

    elements_.clear();
    for (const auto& entry : std::filesystem::directory_iterator(directory_)) {
        elements_.insert(entry.path().lexically_relative(directory_).generic_string());
    }
    return &elements_;
}

const std::set<std::string>* DirectorySet::complementElements() const {
    if (!isMirroring) {
        throw std::logic_error("Unresolveable [Attempted to interact with directory set that is not mirroring]");
    }

    return nullptr;
}

std::string_view DirectorySet::directory() const {
    if (!isMirroring) {
        throw std::logic_error("Unresolveable [Attempted to interact with directory set that is not mirroring]");
    }

    return directory_.c_str();
}

void DirectorySet::saveMachineSubset(std::ostream& saveLocation) const {
    if (!isMirroring) {
        throw std::logic_error("Unresolveable [Attempted to interact with directory set that is not mirroring]");
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
        throw std::logic_error("Resolveable, [Directory mirrored by directory set did not exist]");
    }
    if (!std::filesystem::is_directory(directory_)) {
        throw std::logic_error("Resolveable, [Directory mirrored by directory set is not a directory]");
    }
}