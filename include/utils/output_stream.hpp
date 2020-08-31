#pragma once

#include <sstream>

namespace kvk1920::utils
{

class IOutputStream
{
public:
    virtual ~IOutputStream() = default;


    template <typename ...Args>
    IOutputStream& write(Args&& ...args)
    {
        (writeBuf(std::forward<Args>(args)), ...);
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

    virtual void flushImpl() = 0;

    void flush()
    {
        writeImpl(buff_.str());
        buff_.str(L"");
        flushImpl();
    }
private:
    std::wostringstream buff_;
};

class StdOutputStream final : public IOutputStream
{
public:
    explicit StdOutputStream(std::wostream* out) : out_{out} {}

protected:

protected:
    void writeImpl(const std::wstring& s) final
    {
        *out_ << s;
    }

    void flushImpl() final
    {
        out_->flush();
    }
private:
    std::wostream* out_;
};

}