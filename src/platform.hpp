#pragma once

#include <istream>
#include <ostream>
#include <sstream>
#include <iostream>
#include <string>
#include <filesystem>

#ifdef _WIN32

typedef std::wstring pstring;
typedef std::wstring_view pstring_view;
typedef wchar_t pchar;

#define PASTE(x,y) x##y
#define pliteral(string) PASTE(L,string)

void pCinInsertion(pstring& insert);
template <typename T>
void pCinInsertion(T& insert)  {
    pstring insertProxy;
    pCinInsertion(insertProxy);
    std::wstringstream wideStringStream;
    wideStringStream << insertProxy;
    wideStringStream >> insert; 
}
void pCinGetLine(pstring& line, pstring delimeter = L"\r\n");
void pCinGetLine(std::filesystem::path& path, pstring delimeter = L"\r\n");

void pCoutExtraction(char extract);
void pCoutExtraction(const std::string& extract);
void pCoutExtraction(const char* extract);
void pCoutExtraction(std::string_view extract);
void pCoutExtraction(pchar extract);
void pCoutExtraction(const pstring& extract);
void pCoutExtraction(const pchar* extract);
void pCoutExtraction(pstring_view extract);
template <typename T>
void pCoutExtraction(const T& extract) {
    pstring extractProxy;
    std::wstringstream wideStringStream;
    wideStringStream << extract;
    wideStringStream >> extractProxy;
    pCoutExtraction(extractProxy);
}

// With templates for both pCinInsertion and pCoutExtraction, we can now pretend they're real
struct pcin_ {};
template <typename T>
pcin_& operator>>(pcin_& pcin__, T& insert) {
    pCinInsertion(insert);
    return pcin__;
}
extern pcin_ pcin;

struct pcout_ {};
template <typename T>
pcout_& operator<<(pcout_& pcout__, const T& extract) {
    pCoutExtraction(extract);
    return pcout__;
}
extern pcout_ pcout;

#endif

#ifdef linux

typedef std::string pstring;
typedef std::string_view pstring_view;
typedef char pchar;

#define pcin std::cin
#define pcout std::cout

#define pliteral(string) string

void pCinGetLine(pstring& line, pstring delimeter = "\n");
void pCinGetLine(std::filesystem::path& path, pstring delimeter = "\n");

#endif

void platform();
pchar ptolower(pchar character);
pchar ptoupper(pchar character);
std::string genericStringFromPString(const pstring& string);
std::string genericStringFromPString(pstring_view string);
pstring genericStringToPString(const std::string& string);
pstring genericStringToPString(std::string_view string);
void pCinGet(pchar& character);
void pCinIgnoreAll();