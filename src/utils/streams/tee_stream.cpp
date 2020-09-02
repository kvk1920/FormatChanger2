#include <utils/streams/tee_stream.hpp>

namespace kvk1920::utils
{

TeeStream::TeeStream()
: std::wostream(&tee_buf_)
{}

TeeStream&
TeeStream::addStream(std::wostream& stream)
{
    tee_buf_.addBuffer(stream.rdbuf());
    return *this;
}

}