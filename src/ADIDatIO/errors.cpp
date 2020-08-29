#include "errors.hpp"

#include <stdexcept>
#include <vector>


namespace ADIDatIO::detail
{

namespace
{
thread_local
std::vector<wchar_t> g_buff;

inline const wchar_t*
getErrorMessage(ADIResultCode code)
{
    long necessary_buffer_size;
    if (kResultBufferTooSmall == ADI_GetErrorMessage(code, g_buff.data(), g_buff.size(), &necessary_buffer_size))
    {
        g_buff.resize(necessary_buffer_size + 1);
        if (kResultSuccess != ADI_GetErrorMessage(code, g_buff.data(), g_buff.size(), &necessary_buffer_size))
            return nullptr;
    }
    return g_buff.data();
}

}


[[noreturn]] void
throwException(ADIResultCode code)
{
    if (auto message{getErrorMessage(code)}; message != nullptr)
        throw std::runtime_error{std::string(message, message + std::wcslen(message))};
    throw std::runtime_error{"unknown ADDatIO error"};
}

void
checkException(ADIResultCode code)
{
    if (code != kResultSuccess)
        throwException(code);
}

}
