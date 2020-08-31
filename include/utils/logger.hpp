#pragma once

#include <ostream>

namespace kvk1920::utils
{

class Logger : public std::wostream
{
public:
    class LoggerWrapper : public std::wostream
    {

    };
private:
    std::wostream* out_;
};

}