#pragma once

#include <utils/output_stream.hpp>

namespace kvk1920::utils
{

class Logger : public IOutputStream
{
public:
    explicit Logger(std::wostream* out);

protected:
    void writeImpl(const std::wstring& s) override;
    void flushImpl() override;

private:
    std::wostream* out_;
};

}