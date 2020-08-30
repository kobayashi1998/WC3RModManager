#include "config.h"
#include "utils.h"
#include <QCoreApplication>
#include <QDir>

Config::Config()
{
    //Set Paths
    std::string exePath = QCoreApplication::applicationDirPath().toStdString();
    cfgPath = exePath+"/config.cfg";
    modPath = exePath+"/mods";
    outFilesPath = exePath+"/out_files.txt";
    backupFilesPath = exePath+"/backup_files.txt";

    //Load config file
    utils::TxtReader txtReader(cfgPath);
    while(txtReader.next())
    {
        std::pair<std::string, std::string> setting = line2setting(txtReader.line);
        setSetting(setting.first, setting.second);
    }

    //Load defaults
    bool configChanged = false;

    if(getSetting("GamePath") == "")
    {
        std::string gamePath = "";
        for(int i=0; gamePath == ""; i++)
            switch(i)
            {
                case(0): gamePath = utils::regGet(L"GamePath", REG_SZ);
                         break;
                case(1): gamePath = utils::regGet(L"InstallPath", REG_SZ);
                         break;
                case(2): gamePath = utils::regGetPF86();
                         if(gamePath != "") gamePath += "/Warcraft III";
                         break;
                default: gamePath = "D:/Program Files (x86)/Battle/Warcraft III";
            }

        setSetting("GamePath", gamePath);
        configChanged = true;
    }
    if(getSetting("hideEmptyMods") == "")
    {
        setSetting("hideEmptyMods", "1");
        configChanged = true;
    }

    if(configChanged) saveConfig();
}

void Config::setSetting(std::string key, std::string value)
{
    if(key != "")
    {
        if(value == "") deleteSetting(key);
        else
        {
            if(key == "GamePath") replace(value.begin(), value.end(), '\\', '/');
            if(settings.find(key) == settings.end()) settings.insert({ key, value });
            else settings.find(key)->second = value;
        }
     }
}

void Config::deleteSetting(std::string key)
{
    if(settings.find(key) != settings.end())
        settings.erase(key);
}

std::string Config::getSetting(std::string key)
{
    if(settings.find(key) == settings.end())
        return "";
    else return settings.find(key)->second;
}

std::pair<std::string, std::string> Config::line2setting(std::string line)
{
    size_t pos = line.find_first_of('=');

    std::string field = line.substr(0, pos),
                value = line.substr(pos+1);

    return { field, value };
}

void Config::saveConfig()
{
    std::ofstream cfg(cfgPath.c_str());

    for(std::map<std::string, std::string>::const_iterator it = settings.begin(); it != settings.end(); ++it)
        cfg << it->first+"="+it->second << std::endl;

    cfg.close();
}
