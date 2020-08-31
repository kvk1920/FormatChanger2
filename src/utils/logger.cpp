#include <utils/logger.hpp>

#include <chrono>

namespace kvk1920::utils
{

Logger::Logger(std::wostream* out) : out_{out}
{}

void
Logger::writeImpl(const std::wstring& s)
{
    const auto now{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
    *out_ << "[ " << std::ctime(&now) << "]\t";
    *out_ << s << std::endl;
}

}