#pragma once

#include <istream>
#include <ostream>

class copyformat_ {
    friend std::ostream& operator<<(std::ostream& ostr, const copyformat_&);
};

const copyformat_ copyformat;

class replaceformat_ {
    friend std::ostream& operator<<(std::ostream& ostr, const replaceformat_&);
};

const replaceformat_ replaceformat;

bool insensitiveSame(const std::string& str1, const std::string& str2);

void skipRead(std::istream& istr, size_t count);
void ignoreAll(std::istream& istr);