#ifndef CONFIG_H
#define CONFIG_H

#include <map>

class Config
{
    std::string cfgPath;
    std::map<std::string, std::string> settings;
    std::pair<std::string, std::string> line2setting(std::string);

public:
    Config();

    void setSetting(std::string, std::string);
    std::string getSetting(std::string);
    void deleteSetting(std::string);
    void saveConfig();

    std::string modPath;
    std::string outFilesPath;
    std::string backupFilesPath;
};

#endif // CONFIG_H
