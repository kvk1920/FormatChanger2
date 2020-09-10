#include <utility>
#include <utils/config/var.hpp>

namespace utils::config
{

IVar::IVar(std::string name, const std::type_info& type)
: name_{std::move(name)}
, type_{&type}
{}

const std::string&
IVar::name() const noexcept
{
    return name_;
}

bool
IVar::isEmpty() const
{
    return !value_.has_value() && !default_value_.has_value();
}

bool
IVar::hasValue() const
{
    return value_.has_value() || default_value_.has_value();
}

}