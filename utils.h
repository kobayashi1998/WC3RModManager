#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <fstream>
#include <QString>

namespace utils
{
    std::string i2s(int);
    std::string narrow_str(const std::wstring&);

    std::string regGet(const wchar_t*, DWORD);
    bool regSet(const wchar_t*, DWORD);
    std::string regGetPF86();

    class TxtReader
    {
        std::ifstream txtReader;
        bool invalidPath = false;

        public:
            TxtReader(std::string);
            ~TxtReader();
            std::string line;
            bool next();
    };
};

#endif // UTILS_H
