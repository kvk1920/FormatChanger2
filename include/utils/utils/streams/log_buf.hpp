#pragma once

#include <streambuf>

namespace utils::streams
{

class LogBuf : public std::wstreambuf
{
public:
    explicit LogBuf(std::wstreambuf* buf);

protected:
    int sync() override;
    std::streamsize xsputn(const char_type* s, std::streamsize n) override;
    int_type overflow(int_type c) override;

    bool insertTimestamp();
protected:
    std::wstring time_buff_;
    bool at_start_of_line_{true};
    std::wstreambuf* buf_{nullptr};
};

}