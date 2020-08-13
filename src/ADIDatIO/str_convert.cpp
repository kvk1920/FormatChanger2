#include <cstring>
#include <vector>

namespace ADIDatIO::detail
{

namespace {
thread_local
std::vector<char> buff;
}

const char* wcstring_to_cstring(const wchar_t* string)
{
    if (nullptr == string)
        return nullptr;
    buff.resize(static_cast<size_t>(wcslen(string)) + 1);
    for (size_t i{0}; i + 1 < buff.size(); ++i)
        buff[i] = static_cast<char>(string[i]);
    buff.back() = '\0';
    return buff.data();
}

}
