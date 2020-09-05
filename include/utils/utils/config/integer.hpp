#pragma once

#include <utils/config/var.hpp>

namespace utils::config
{

class Integer final : public IVar
{
public:
    explicit Integer(std::string name);

    void load(std::wistream& in) final;
    void save(std::wostream& out) const final;
};

}