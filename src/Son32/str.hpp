#pragma once

#include <string>
#include <filesystem>

namespace Son32
{

[[nodiscard]] inline
std::string wstringToString(const std::wstring& wstr);

}

[[nodiscard]] std::string
Son32::wstringToString(const std::wstring& wstr)
{
    std::string str;
    for (auto c : wstr)
    {
        if (static_cast<int>(c) > 127)
            str.push_back('_');
        else
            str.push_back(c);
        if (isblank(str.back()))
            str.back() = '_';
    }
    return str;
}