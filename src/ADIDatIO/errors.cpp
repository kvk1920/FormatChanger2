#include <errors.hpp>

#include <stdexcept>
#include <vector>


namespace ADIDatIO::detail
{

namespace
{

inline std::wstring
getErrorMessage(ADIResultCode code)
{
    std::vector<wchar_t> buff;
    long necessary_buffer_size;
    if (kResultBufferTooSmall == ADI_GetErrorMessage(code, buff.data(), buff.size(), &necessary_buffer_size))
    {
        buff.resize(necessary_buffer_size + 1);
        if (kResultSuccess != ADI_GetErrorMessage(code, buff.data(), buff.size(), &necessary_buffer_size))
            return L"";
    }
    return buff.data();
}

}


[[noreturn]] void
throwException(ADIResultCode code)
{
    if (auto message{getErrorMessage(code)}; !message.empty())
    {
        auto t{std::string(message.begin(), message.end())};
        throw std::runtime_error{std::move(t)};
    }
    throw std::runtime_error{"unknown ADDatIO error"};
}

void
checkException(ADIResultCode code)
{
    if (code & ADIResultCode::kResultErrorFlagBit)
        throwException(code);
}

}
