#include "utils.h"
#include <sstream>
#include <QFileInfo>
#include <QDir>

namespace utils {
    std::string i2s(int i)
    {
        std::ostringstream stream;
        stream << i;
        return stream.str();
    }

    std::string narrow_str(const std::wstring& str)
    {
        std::ostringstream stm;
        const std::ctype<char>& ctfacet = std::use_facet< std::ctype<char> >(stm.getloc());
        for(size_t i=0; i<str.size(); ++i) stm << ctfacet.narrow(char(str[i]), 0);
        return stm.str();
    }

    bool regOpenKey(REGSAM samDesired, HKEY* hKey)
    {
        if(RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Blizzard Entertainment\\Warcraft III", 0, samDesired, hKey) == ERROR_SUCCESS)
            return true;
        else return false;
    }

    std::string regGet(const wchar_t* key, DWORD type)
    {
        HKEY hKey;
        std::string sResult = "";
        if(regOpenKey(KEY_READ, &hKey))
        {
            DWORD size=1024;

            if(type == REG_SZ)
            {
                wchar_t result[MAX_PATH];
                if(RegQueryValueEx(hKey, key, nullptr, &type, LPBYTE(&result), &size) == ERROR_SUCCESS)
                    sResult = narrow_str(result);
            }
            else if(type == REG_DWORD)
            {
                DWORD result;
                if(RegQueryValueEx(hKey, key, nullptr, &type, LPBYTE(&result), &size) == ERROR_SUCCESS)
                    sResult = i2s(static_cast<int>(result));
            }
        }

        RegCloseKey(hKey);

        return sResult;
    }


    bool regSet(const wchar_t* key, DWORD value)
    {
        HKEY hKey;
        bool success = false;

        if(regOpenKey(KEY_ALL_ACCESS, &hKey))
            success = (RegSetValueEx(hKey, key, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value)) == ERROR_SUCCESS);

        RegCloseKey(hKey);

        return success;
    }

    std::string regGetPF86()
    {
        HKEY hKey;
        std::string sResult = "";
        if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Winwdows\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            wchar_t result[MAX_PATH];
            DWORD size = 1024, type = REG_SZ;
            if(RegQueryValueEx(hKey, L"ProgramFilesDir (x86)", nullptr, &type, LPBYTE(&result), &size) == ERROR_SUCCESS)
                sResult = narrow_str(result);
        }

        RegCloseKey(hKey);

        return sResult;
    }

    TxtReader::TxtReader(std::string path)
    {
        QFileInfo fiPath(QString::fromStdString(path));

        if(fiPath.exists() && fiPath.isFile())
        {
            txtReader.open(path.c_str());
        }
        else invalidPath = true;
    }

    bool TxtReader::next()
    {
        if(invalidPath || !txtReader)
        {
            if(!invalidPath && txtReader.is_open()) txtReader.close();
            return false;
        }
        else
        {
            getline(txtReader, line);
            return true;
        }
    }

    TxtReader::~TxtReader()
    {
        if(txtReader.is_open()) txtReader.close();
    }
}
