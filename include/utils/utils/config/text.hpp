#pragma once

#include <utils/config/var.hpp>

namespace utils::config
{

class Text final : public IVar
{
public:
    explicit Text(std::string name);

    void load(std::wistream& in) final;
    void save(std::wostream& out) const final;

    void setString(std::wstring value);
};

}