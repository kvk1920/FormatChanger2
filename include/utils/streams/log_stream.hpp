#pragma once

#include <utils/streams/log_buf.hpp>

#include <ostream>

namespace kvk1920::utils
{

class LogStream : public std::wostream
{
public:
    explicit LogStream(std::wstreambuf* buff);
    explicit LogStream(std::wostream& stream);
private:
    LogBuf buf_;
};

}