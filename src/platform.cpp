#include "platform.hpp" 

#ifdef _WIN32

#include "windows.h"

std::string denativePath(const std::filesystem::path& path) {
    return std::string(reinterpret_cast<const char*>(path.u8string().data()));
}

nstring nativeString(const std::string& utf8String) {
    return nativeString(std::string_view(utf8String));
}
nstring nativeString(std::string_view utf8String) {
    nstring nativeString;
    nativeString.resize(utf8String.size());
    int size = MultiByteToWideChar(CP_UTF8, 0, utf8String.data(), utf8String.size(), nativeString.data(), nativeString.size());
    nativeString.resize(size);
    return nativeString;
}

#endif

#ifdef linux

std::string denativePath(const std::filesystem::path& path) {
    return std::string(reinterpret_cast<const char*>(path.u8string().data()));
}

nstring nativeString(const std::string& utf8String) {
    return utf8String;
}
nstring nativeString(std::string_view utf8String) {
    return nstring(utf8String);
}

#endif