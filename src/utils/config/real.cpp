#include <utils/config/real.hpp>

namespace utils::config
{

Real::Real(std::string name)
: IVar(std::move(name), typeid(double))
{}

void
Real::save(std::wostream& out) const
{
    out << get<double>();
}

void
Real::load(std::wistream& in)
{
    double value;
    in >> value;
    value_ = value;
}

}