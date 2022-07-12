/*
    helpers.hpp

    Contains useful utilities to avoid having one-purpose files for simple things
*/
#pragma once

#include "platform.hpp"

#include <istream>
#include <ostream>
#include <numeric>

class copyformat_ {
    friend std::ostream& operator<<(std::ostream& ostr, const copyformat_&);
};

const copyformat_ copyformat;

class replaceformat_ {
    friend std::ostream& operator<<(std::ostream& ostr, const replaceformat_&);
};

const replaceformat_ replaceformat;

bool insensitiveSame(pstring_view str1, pstring_view str2);

void skipRead(std::istream& istr, size_t count);

void ignoreAll(std::istream& istr);