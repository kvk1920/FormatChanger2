#include <ADIDatIO/detail/errors.h>
#include <ADIDatIO/detail/str_convert.h>

#include <stdexcept>
#include <vector>


namespace ADIDatIO::detail
{

namespace
{
thread_local
std::vector<wchar_t> buff;

inline const wchar_t* get_error_message(ADIResultCode code)
{
    long sz;
    if (kResultBufferTooSmall == ADI_GetErrorMessage(code, buff.data(), buff.size(), &sz))
    {
        buff.resize(sz + 1);
        if (kResultSuccess != ADI_GetErrorMessage(code, buff.data(), buff.size(), &sz))
            return nullptr;
    }
    return buff.data();
}
}


[[noreturn]]
void throw_exception(ADIResultCode code)
{
    if (auto msg{wcstring_to_cstring(get_error_message(code))}; msg != nullptr)
        throw std::runtime_error{msg};
    throw std::runtime_error{"unknown ADDatIO error"};
}

}
