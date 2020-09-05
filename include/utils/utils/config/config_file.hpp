#pragma once

#include <utils/config/var.hpp>

#include <memory>
#include <unordered_map>

namespace utils::config
{

class ConfigFile
{
public:
    static ConfigFile& instance();
    static const ConfigFile& constInstance();
    ConfigFile& createVar(std::unique_ptr<IVar> var);
    IVar* loadVar(const std::string& name);
    const IVar* loadVar(const std::string& name) const;
    ConfigFile& loadFile();
    const ConfigFile& saveFile() const;
private:
    static ConfigFile instance_;

    std::unordered_map<std::string, std::unique_ptr<IVar>> vars_;
};

}