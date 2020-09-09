#pragma once

#include <any>
#include <iostream>
#include <string>

namespace utils::config
{

class IVar
{
protected:
    explicit IVar(std::string name, const std::type_info& type);
public:
    ~IVar() = default;
    [[nodiscard]]
    const std::string& name() const noexcept;

    virtual void load(std::wistream& in) = 0;
    virtual void save(std::wostream& out) const = 0;

    template <typename T>
    void setDefault(const T& value);

    template <typename T>
    [[nodiscard]]
    const T& get() const;

    template <typename T>
    void set(const T& value);

    [[nodiscard]] bool hasValue() const;
    [[nodiscard]] bool isEmpty() const;
protected:
    std::string name_;
    std::any value_;
    std::any default_value_;
    const std::type_info* type_;
};

}

template <>
struct std::hash<utils::config::IVar>
{
    std::size_t operator()(const utils::config::IVar& var) const noexcept
    {
        return std::hash<std::string>()(var.name());
    }
};

namespace utils::config
{
template <typename T>
const T& IVar::get() const
{
    if (!value_.has_value())
        return std::any_cast<const T&>(default_value_);
    return std::any_cast<const T&>(value_);
}

template <typename T>
void IVar::set(const T& value)
{
    if (*type_ != typeid(T))
        throw std::logic_error(""); // TODO: error message
    value_ = value;
}

template <typename T>
void IVar::setDefault(const T& value)
{
    if (*type_ != typeid(T))
        throw std::logic_error(""); // TODO: error message
    default_value_ = value;
}

}