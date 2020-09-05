#include <utils//config/boolean.hpp>

namespace utils::config
{

Boolean::Boolean(std::string name)
: IVar(std::move(name), typeid(bool))
{
}

void
Boolean::load(std::wistream& in)
{
    std::wstring word;
    in >> word;
    if (L"true" == word)
        value_ = true;
    else if (L"false" == word)
        value_ = false;
    else
        throw std::logic_error(""); // TODO: error message
}

void
Boolean::save(std::wostream& out) const
{
    static const wchar_t* words[] = {
            L"false", L"true"
    };
    out << words[get<bool>()];
}

}