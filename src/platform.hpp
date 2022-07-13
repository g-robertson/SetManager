#include <string>
#include <filesystem>

#ifdef _WIN32

typedef std::wstring nstring;
typedef std::wstring_view nstring_view;

#endif

#ifdef linux

typedef std::string nstring;
typedef std::string_view nstring_view;

#endif

std::string denativePath(const std::filesystem::path& path);

nstring nativeString(const std::string& utf8String);
nstring nativeString(std::string_view utf8String);