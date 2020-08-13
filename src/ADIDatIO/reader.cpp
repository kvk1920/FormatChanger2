#include <ADIDatIO/detail/errors.h>
#include <ADIDatIO/detail/str_convert.h>
#include <ADIDatIO/reader.h>

#include <stdexcept>

namespace ADIDatIO
{
using namespace detail;

/*void Reader::ensure(std::string op)
{
    if (nullptr == _handle)
        throw std::runtime_error("ADIDatIO: " + op + ": file needs to be opened");
}*/

Reader::Reader()
    : _handle{nullptr}
{
}

Reader::Reader(const wchar_t* path)
    : Reader()
{
    open(path);
}

Reader::~Reader()
{
    close();
}

void Reader::open(const wchar_t* path)
{
    close();
    _handle = new ADI_FileHandle;
    check_exception(ADI_OpenFile(path, _handle, kOpenFileForReadOnly));
}

void Reader::close()
{
    if (_handle)
    {
        ADI_CloseFile(_handle);
        delete _handle;
        _handle = nullptr;
    }
}

size_t Reader::get_number_of_records()
{
    long num_records;
    check_exception(ADI_GetNumberOfRecords(*_handle, &num_records));
    return num_records;
}

size_t Reader::get_number_of_channels()
{
    long num_channels;
    check_exception(ADI_GetNumberOfChannels(*_handle, &num_channels));
    return num_channels;
}

namespace
{
thread_local std::vector<wchar_t> buff;

inline std::string get_name(ADI_FileHandle* handle, size_t channel, ADIResultCode f(ADI_FileHandle, long, wchar_t*, long, long*))
{
    long length;
    ADIResultCode code{f(*handle, channel, buff.data(), buff.size(), &length)};
    if (kResultBufferTooSmall == code)
    {
        buff.resize(length + 1);
        code = f(*handle, channel, buff.data(), buff.size(), &length);
    }
    check_exception(code);
    if (buff.size() == static_cast<size_t>(length))
        buff.push_back('\0');
    else
        buff[length] = '\0';
    return wcstring_to_cstring(buff.data());
}

ADIResultCode _get_units_name(ADI_FileHandle handle, long channel, wchar_t* name, long name_length, long* real_length)
{
    return ADI_GetUnitsName(handle, channel, 0, name, name_length, real_length);
}
}

std::string Reader::get_channel_name(size_t channel)
{
    return get_name(_handle, channel, ADI_GetChannelName);
}

std::string Reader::get_units_name(size_t channel)
{
    return get_name(_handle, channel, _get_units_name);
}

std::vector<float> Reader::get_record(size_t channel, size_t record)
{
    long sz;
    check_exception(ADI_GetNumSamplesInRecord(*_handle, channel, record, &sz));
    std::vector<float> result(sz);
    for (long pos{0}; static_cast<size_t>(pos) < result.size();)
    {
        check_exception(ADI_GetSamples(*_handle, channel, record, pos, kADICDataAtSampleRate, result.size(), result.data() + pos, &sz));
        pos += sz;
    }
    return result;
}

double Reader::get_channel_sample_period(size_t channel)
{
    double result;
    check_exception(ADI_GetRecordSamplePeriod(*_handle, channel, 0, &result));
    return result;
}

namespace
{
char UNUSED_BUFF[256];
}

Reader::Time Reader::get_record_time(size_t record)
{
    Time result;
    check_exception(ADI_GetRecordTime(*_handle, record, &result.seconds, &result.frac_seconds, reinterpret_cast<long*>(UNUSED_BUFF)));
    return result;
}

}
