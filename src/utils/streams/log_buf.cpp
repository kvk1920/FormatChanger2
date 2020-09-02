#include <utils/streams/log_buf.hpp>

#include <chrono>

#include <cstdio>
#include <cstring>

namespace kvk1920::utils
{

LogBuf::LogBuf(std::wstreambuf* buf)
{
    buf_ = buf;
    time_buff_.reserve(64);
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
        if (!nxt_pos)
            nxt_pos = s + n;
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
    const char* result{std::ctime(&now)};
    buff.assign(L"[ ");
    buff.append(result, result + strlen(result));
    if (L'\n' == buff.back())
        buff.pop_back();
    buff.append(L" ]\t");
}

}

bool
LogBuf::insertTimestamp()
{
    getCurrentTime(time_buff_);
    return time_buff_.size() == buf_->sputn(time_buff_.data(), time_buff_.size());
}

}