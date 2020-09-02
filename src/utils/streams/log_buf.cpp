#include <utils/streams/log_buf.hpp>

#include <chrono>

namespace kvk1920::utils
{

LogBuf::LogBuf(std::wstreambuf* buf)
{
    buf_ = buf;
}

int
LogBuf::sync()
{
    return buf_->pubsync();
}

std::streamsize
LogBuf::xsputn(const char_type* s, std::streamsize n)
{

    const wchar_t* cur_pos{s};
    std::streamsize rem{n};
    while (cur_pos < s + n)
    {
        if (at_start_of_line_)
            if (!insertTimestamp())
                return 0;
        at_start_of_line_ = false;
        const wchar_t* nxt_pos{std::wmemchr(cur_pos, L'\n', rem)};
        if (nxt_pos < s + n)
        {
            ++nxt_pos;
            at_start_of_line_ = true;
        }
        buf_->sputn(s, nxt_pos - cur_pos);
        cur_pos = nxt_pos;
        rem = s + n - cur_pos;
    }
    return n;
}

LogBuf::int_type
LogBuf::overflow(int_type c)
{
    if (at_start_of_line_)
        if (!insertTimestamp())
            return traits_type::eof();
    at_start_of_line_ = L'\n' == c;
    return buf_->sputc(c);
}

namespace
{

void
getCurrentTime(std::wstring& buff)
{
    auto now{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
    auto local_time{*std::localtime(&now)};
    std::size_t result_code{0};
    while (!result_code)
    {
        buff.resize(std::max<std::size_t>(1, buff.size() * 2));
        result_code = std::wcsftime(buff.data(), buff.size(),
                      L"%F %T", &local_time);
    }
    buff.resize(result_code);
}

}

bool
LogBuf::insertTimestamp()
{
    getCurrentTime(time_buff_);
    return time_buff_.size() == buf_->sputn(time_buff_.data(), time_buff_.size());
}

}