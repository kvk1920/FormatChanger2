#pragma once

#include <utils/config/var.hpp>

namespace utils::config
{

class Boolean final : public IVar
{
public:
    explicit Boolean(std::string name);

    void load(std::wistream& in) final;
    void save(std::wostream& out) const final;
};

}