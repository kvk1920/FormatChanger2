#include <utils/streams/log_stream.hpp>

namespace kvk1920::utils
{

LogStream::LogStream(std::wstreambuf* buff)
: std::wostream(&buf_)
, buf_{buff}
{

}

LogStream::LogStream(std::wostream& stream)
: LogStream(stream.rdbuf())
{
}

}