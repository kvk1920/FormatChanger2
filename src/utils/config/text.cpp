#include <utils/config/text.hpp>

namespace utils::config
{

Text::Text(std::string name)
: IVar(std::move(name), typeid(std::wstring))
{}

void
Text::load(std::wistream& in)
{
    std::wstring value;
    wchar_t c;
    do
    {
        c = in.get();
    }
    while (c != L'"');
    do
    {
        c = in.get();
        value.push_back(c);
    }
    while (c != L'"');
    value.pop_back();
    value_ = std::move(value);
}

void
Text::save(std::wostream& out) const
{
    out.put(L'"') << get<std::wstring>();
    out.put(L'"');
}

void
Text::setString(std::wstring value)
{
    IVar::set(value);
}

}