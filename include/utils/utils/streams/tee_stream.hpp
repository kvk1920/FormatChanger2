#pragma once

#include <utils/streams/tee_buf.hpp>

namespace utils::streams
{

class TeeStream final : public std::wostream
{
public:
    TeeStream();

    TeeStream& addStream(std::wostream& stream);
private:
    TeeBuf tee_buf_;
};

}