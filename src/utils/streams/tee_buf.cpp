#include <utils/streams/tee_buf.hpp>

namespace utils::streams
{

TeeBuf::int_type
TeeBuf::overflow(int_type c)
{
    for (auto buffer : buffers_)
    {
        if (traits_type::eof() == buffer->sputc(c))
            return traits_type::eof();
    }
    return c;
}

std::streamsize
TeeBuf::xsputn(const char_type* s, std::streamsize n)
{
    std::streamsize result{n};
    for (auto buffer : buffers_)
        result = std::min(result, buffer->sputn(s, n));
    return result;
}

int
TeeBuf::sync()
{
    int result{0};
    for (auto buffer : buffers_)
        result = std::min(buffer->pubsync(), result);
    return result;
}

TeeBuf&
TeeBuf::addBuffer(std::wstreambuf* buff)
{
    buffers_.push_back(buff);
    return *this;
}

}