#pragma once

#include <utils/config/var.hpp>

namespace utils::config
{

class Real final : public IVar
{
public:
    explicit Real(std::string name);

    void save(std::wostream& out) const final;
    void load(std::wistream& in) final;
};

}