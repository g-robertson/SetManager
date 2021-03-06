/*
    helpers.cpp

    Contains useful utilities to avoid having one-purpose files for simple things
*/
#include "helpers.hpp"

#include <stack>
#include <iomanip>
#include <memory>

namespace {
    std::stack<std::unique_ptr<std::ios>> formats;
    char SKIP[4096];
}

std::ostream& operator<<(std::ostream& ostr, const copyformat_&) {
    formats.push(std::make_unique<std::ios>(nullptr));
    formats.top()->copyfmt(ostr);
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const replaceformat_&) {
    if (formats.size() == 0) {
        throw std::logic_error("Unappearable, [Tried to pop a format when none have been pushed]");
    }
    ostr.copyfmt(*formats.top());
    formats.pop();
    return ostr;
}


bool insensitiveSame(std::string_view str1, std::string_view str2) {
    return str1.size() == str2.size() && std::equal(str1.begin(), str1.end(), str2.begin(), str2.end(), [](auto char1, auto char2) {
        return std::toupper(char1) == std::toupper(char2);
    });
}

void skipRead(std::istream& istr, size_t count) {
    if (count > 4096) {
        throw std::logic_error("Not allowed to skip more than 4096 with skipRead");
    }
    istr.read(SKIP, count);
}

void ignoreAll(std::istream& istr) {
    istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}