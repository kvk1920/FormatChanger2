#include <utils/config/config_file.hpp>

#include <fstream>
#include <sstream>

namespace utils::config
{

ConfigFile ConfigFile::instance_;

ConfigFile&
ConfigFile::instance()
{
    return instance_;
}

const ConfigFile&
ConfigFile::constInstance()
{
    return instance_;
}

ConfigFile&
ConfigFile::createVar(std::unique_ptr<IVar> var)
{
    std::string name{var->name()};
    vars_[name] = std::move(var);
    return *this;
}

IVar*
ConfigFile::loadVar(const std::string& name)
{
    if (auto it{vars_.find(name)}; it != vars_.end())
        return it->second.get();
    return nullptr;
}

const IVar*
ConfigFile::loadVar(const std::string& name) const
{
    if (auto it{vars_.find(name)}; it != vars_.end())
        return it->second.get();
    return nullptr;
}

ConfigFile&
ConfigFile::loadFile()
{
    std::wifstream fin("config.txt");
    std::wstring line;
    while (std::getline(fin, line))
    {
        const auto sep{line.find(L'=')};
        std::string name(line.begin(), line.begin() + sep);
        const auto var{vars_.find(name)};
        std::wistringstream var_value(line.substr(sep + 1));
        if (var != vars_.end())
            var->second->load(var_value);
    }
    return *this;
}

const ConfigFile&
ConfigFile::saveFile() const
{
    std::wofstream fout("config.txt");
    for (const auto& [name, var] : vars_)
    {
        fout << name.c_str() << '=';
        var->save(fout);
    }
    fout.flush();
    return *this;
}

}