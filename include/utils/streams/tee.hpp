#pragma once

#include <ostream>
#include <vector>

namespace kvk1920::utils
{

class TeeBuf final : public std::wstreambuf
{
public:
    TeeBuf& addBuffer(std::wstreambuf* buff);

protected:
    int_type overflow(int_type c) final;
    std::streamsize xsputn(const char_type* s, std::streamsize n) final;
    int sync() final;
private:
    std::vector<std::wstreambuf*> buffers_;
};

class TeeStream final : public std::wostream
{
public:
    TeeStream();

    TeeStream& addStream(std::wostream& stream);
private:
    TeeBuf tee_buf_;
};

}