#include "platform.hpp"


#ifdef _WIN32

#include "windows.h"

#include <unordered_set>

pcin_ pcin;
pcout_ pcout;

auto WinCin = GetStdHandle(STD_INPUT_HANDLE);
auto WinCout = GetStdHandle(STD_OUTPUT_HANDLE);
auto buffer = pstring(4096, '0');
DWORD bufferStart = 0;
DWORD bufferEnd = 0;
std::unordered_set<pchar> WHITESPACE = {
    ' ', '\t', '\r', '\n', '\0'
};

void platform() {
}

bool ignoredAll = false;

void pCinIgnoreAll_() {
    do {
        auto bufferView = pstring_view(buffer.begin() + bufferStart, buffer.begin() + bufferEnd);
        for (size_t i = 0; i < bufferView.size(); ++i) {
            pchar character = bufferView[i];
            if (WHITESPACE.find(character) != WHITESPACE.end()) {
                continue;
            }
            bufferStart += i;
            return;
        }

        bufferStart = 0;
        bufferEnd = 0;

        // if there are no console input events then wait until there are
        DWORD inputEvents;
        GetNumberOfConsoleInputEvents(WinCin, &inputEvents);
        if (inputEvents == 0) {
            WaitForSingleObject(WinCin, INFINITE);
        }
        // then read it into buffer
        ReadConsoleW(WinCin, buffer.data(), 4096, &bufferEnd, NULL);
    } while (true);
    ignoredAll = false;
}

void pCinInsertion(pstring& insert) {
    if (ignoredAll) {
        pCinIgnoreAll_();
    }
    insert.erase();
    bool preContent = true;
    do {
        auto bufferView = pstring_view(buffer.begin() + bufferStart, buffer.begin() + bufferEnd);
        int afterPreContent = 0;
        for (size_t i = 0; i < bufferView.size(); ++i) {
            pchar character = bufferView[i];
            if (WHITESPACE.find(character) != WHITESPACE.end()) {
                if (preContent) {
                    afterPreContent = i + 1;
                    continue;
                }

                bufferStart += i + 1;
                insert += pstring(bufferView.substr(afterPreContent, i));
                return;
            } else {
                preContent = false;
            }
        }

        insert += bufferView.substr(afterPreContent);
        bufferStart = 0;
        bufferEnd = 0;

        // if there are no console input events then wait until there are
        DWORD inputEvents;
        GetNumberOfConsoleInputEvents(WinCin, &inputEvents);
        if (inputEvents == 0) {
            WaitForSingleObject(WinCin, INFINITE);
        }
        // then read it into buffer
        ReadConsoleW(WinCin, buffer.data(), 4096, &bufferEnd, NULL);
    } while (true);
}

void pCinGetLine(pstring& line, pstring delimeter) {
    if (ignoredAll) {
        pCinIgnoreAll_();
    }
    line.erase();
    do {
        auto bufferView = pstring_view(buffer.begin() + bufferStart, buffer.begin() + bufferEnd);

        auto index = bufferView.find(delimeter);
        if (index != pstring::npos) {
            bufferStart += index + delimeter.size();
            line += pstring(bufferView.substr(0, index));
            return;
        }

        line += bufferView;
        bufferStart = 0;
        bufferEnd = 0;

        // if there are no console input events then wait until there are
        DWORD inputEvents;
        GetNumberOfConsoleInputEvents(WinCin, &inputEvents);
        if (inputEvents == 0) {
            WaitForSingleObject(WinCin, INFINITE);
        }
        // then read it into buffer
        ReadConsoleW(WinCin, buffer.data(), 4096, &bufferEnd, NULL);
    } while (true);
}

void pCinGetLine(std::filesystem::path& path, pstring delimeter) {
    pstring line;
    pCinGetLine(line, delimeter);
    path = std::filesystem::path(line);
}

void pCinIgnoreAll() {
    ignoredAll = true;
}

void pCinGet(pchar& character) {
    pCinInsertion(character);
}

void pCoutExtraction(const std::string& extract) {
    pCoutExtraction(std::string_view(extract));
}

void pCoutExtraction(const char* extract) {
    pCoutExtraction(std::string_view(extract));
}

void pCoutExtraction(std::string_view extract) {
    pCoutExtraction(genericStringToPString(extract));
}

void pCoutExtraction(char extract) {
    if (extract == '\n') {
        pCoutExtraction(L"\r\n");
    } else {
        pCoutExtraction(genericStringToPString(std::string_view(&extract, 1)));
    }
}

void pCoutExtraction(const pstring& extract) {
    pCoutExtraction(pstring_view(extract));
}

void pCoutExtraction(const pchar* extract) {
    pCoutExtraction(pstring_view(extract));
}

void pCoutExtraction(pstring_view extract) {
    DWORD totalCharactersWritten = 0;
    do {
        DWORD charactersWritten;
        WriteConsoleW(WinCout, extract.data(), extract.size(), &charactersWritten, NULL);
        totalCharactersWritten += charactersWritten;
    } while(totalCharactersWritten < extract.size());
}

void pCoutExtraction(pchar extract) {
    DWORD charactersWritten;
    WriteConsoleW(WinCout, &extract, 1, &charactersWritten, NULL);
}

std::string genericStringFromPString(const pstring& string) {
    return genericStringFromPString(pstring_view(string));
}
std::string genericStringFromPString(pstring_view string) {
    std::string genericString;
    genericString.resize((string.size() * 3) + 1);
    int size = WideCharToMultiByte(CP_UTF8, 0, string.data(), string.size(), genericString.data(), genericString.size(), NULL, NULL);
    genericString.resize(size);
    return genericString;
}
pstring genericStringToPString(const std::string& string) {
    return genericStringToPString(std::string_view(string));
}
pstring genericStringToPString(std::string_view string) {
    pstring pString;
    pString.resize(string.size());
    int size = MultiByteToWideChar(CP_UTF8, 0, string.data(), string.size(), pString.data(), pString.size());
    pString.resize(size);
    return pString;
}

#endif

#ifdef linux

#include "helpers.hpp"

#include <string>

void platform() {
}


void pCinGetLine(pstring& line, pstring delimeter) {
    line.erase();
    bool refutedDelimeter;
    do {
        refutedDelimeter = false;
        std::string lineAdd;
        std::getline(std::cin, lineAdd, delimeter[0]);
        for (size_t i = 1; i < delimeter.size(); ++i) {
            char character;
            std::cin.get(character);
            if (character != delimeter[i]) {
                lineAdd += delimeter.substr(0, i);
                refutedDelimeter = true;
                break;
            }
        }
        line += lineAdd;
    } while (refutedDelimeter);
}

void pCinGetLine(std::filesystem::path& path, pstring delimeter) {
    pstring line;
    pCinGetLine(line, delimeter);
    path = line;
}

void pCinGet(pchar& character) {
    std::cin.get(character);
}
void pCinIgnoreAll() {
    ignoreAll(std::cin);
}

std::string genericStringFromPString(const pstring& string) {
    return string;
}
std::string genericStringFromPString(pstring_view string) {
    return std::string(string);
}
pstring genericStringToPString(const std::string& string) {
    return string;
}
pstring genericStringToPString(std::string_view string) {
    return std::string(string);
}


#endif