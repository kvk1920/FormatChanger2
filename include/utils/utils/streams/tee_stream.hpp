#pragma once

#include <utils/streams/tee_buf.hpp>

namespace kvk1920::utils
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