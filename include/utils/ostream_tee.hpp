#pragma once

#include <utils/output_stream.hpp>

#include <vector>

namespace kvk1920::utils
{

class OutputStreamTee final : public IOutputStream
{
public:
    OutputStreamTee& addStream(IOutputStream* out)
    {
        outs_.push_back(out);
        return *this;
    }

protected:
    void writeImpl(const std::wstring& s) final
    {
        for (auto out : outs_)
            out->write(s);
    }

    void flushImpl() final {}
private:
    std::vector<IOutputStream*> outs_;
};

}