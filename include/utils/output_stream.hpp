#pragma once

#include <sstream>

namespace kvk1920::utils
{

class IOutputStream
{
public:
    virtual ~IOutputStream() = default;


    template <typename ...Args>
    IOutputStream& write(const Args& ...args)
    {
        (writeBuf(args), ...);
        flush();
        return *this;
    }
protected:
    virtual void writeImpl(const std::wstring& s) = 0;

    template <typename T>
    void writeBuf(const T& value)
    {
        buff_ << value;
    }

    void flush()
    {
        writeImpl(buff_.str());
        buff_.str(L"");
    }
private:
    std::wostringstream buff_;
};

class StdOutputStream final : IOutputStream
{
public:
    explicit StdOutputStream(std::wostream* out) : out_{out} {}
protected:
    void writeImpl(const std::wstring& s) final
    {
        *out_ << s;
    }
private:
    std::wostream* out_;
};

}