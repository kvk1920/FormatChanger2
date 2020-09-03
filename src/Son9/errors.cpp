#include <errors.hpp>
#include <ctime>
#include <windows.h>
#include <extern/Son9/Son.h>
#include <stdexcept>

namespace Son32::detail
{

[[noreturn]] void
throwException(long code)
{
    std::string msg;
    switch (code)
    {
    case SON_NO_FILE: msg = "SON_NO_FILE"; break;
    case SON_NO_HANDLES: msg = "SON_NO_HANDLES"; break;
    case SON_NO_ACCESS: msg = "SON_NO_ACCESS"; break;
    case SON_BAD_HANDLE: msg = "SON_BAD_HANDLE"; break;
    case SON_OUT_OF_MEMORY: msg = "SON_OUT_OF_MEMORY"; break;
    case SON_BAD_READ: msg = "SON_BAD_READ"; break;
    case SON_BAD_WRITE: msg = "SON_BAD_WRITE"; break;
    case SON_NO_CHANNEL: msg = "SON_NO_CHANNEL"; break;
    case SON_CHANNEL_USED: msg = "SON_CHANNEL_USED"; break;
    case SON_CHANNEL_UNUSED: msg = "SON_CHANNEL_UNUSED"; break;
    case SON_PAST_EOF: msg = "SON_PAST_EOF"; break;
    case SON_WRONG_FILE: msg = "SON_WRONG_FILE"; break;
    case SON_NO_EXTRA: msg = "SON_NO_EXTRA"; break;
    case SON_CORRUPT_FILE: msg = "SON_CORRUPT_FILE"; break;
    case SON_PAST_SOF: msg = "SON_PAST_SOF"; break;
    case SON_READ_ONLY: msg = "SON_READ_ONLY"; break;
    case SON_BAD_PARAM: msg = "SON_BAD_PARAM"; break;
    default: msg = "SON_UNKNOWN_ERROR";
    }
    throw std::runtime_error("Son32 exception: " + msg);
}

void
checkException(long code)
{
    if (code < 0)
        throwException(code);
}

}
