#include <utils/config/integer.hpp>

namespace utils::config
{

Integer::Integer(std::string name)
: IVar(std::move(name), typeid(int64_t))
{}

void
Integer::load(std::wistream& in)
{
    int64_t value;
    in >> value;
    value_ = value;
}

void
Integer::save(std::wostream& out) const
{
    out << get<int64_t>();
}

}