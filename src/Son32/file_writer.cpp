#include <Son32/file_writer.hpp>
#include <Son32/errors.hpp>
#include <Son32/str.hpp>

#include <windows.h>
#include <ctime>
#include <Son.h>

#include <cmath>
#include <numeric>

#include <iostream>

namespace
{

inline long
calculateOptimalUsPerTime(const std::vector<double>& sample_periods)
{
    long base{1'000'000};
    for (auto sample_period : sample_periods)
        base = std::gcd(base, static_cast<long>(std::round(sample_period * 1'000'000)));
    return base;
}

}

namespace Son32
{
using namespace detail;

FileWriter::OffsetScale
FileWriter::calculateOffsetScale(float min_value, float max_value)
{
    /*
     * real = integer * scale / 6553.6 + offset
     * integer = (real - offset) * 6553.6 / scale
     * 2^16 = real * 6553.6 / scale
     * 2^13 >= real * 6553.6 / scale
     */
    OffsetScale result{0, 1};
    double value{std::max(std::abs(min_value), std::abs(max_value))};
    while ((1u << 13u) < value * 6553.6 / result.scale)
        result.scale *= 2;
    return result;
}

std::size_t
FileWriter::numberOfChannels() const
{
    return write_pos_.size();
}

Time
FileWriter::fileStart() const
{
    return start_;
}

namespace
{

inline std::string
getPrefix(const std::string& str, std::size_t length)
{
    if (str.size() > length)
        return str.substr(0, length);
    return str;
}

}

FileWriter::FileWriter(const Config& config)
{
    path_ = wstringToString(config.path.wstring());
    //std::cout << "path_ = " << path_ << std::endl;
    handle_ = SONCreateFile(path_.c_str(), config.channels.size(), 0);
    start_ = config.start;
    checkException(handle_);
    write_pos_.assign(config.channels.size(), 0);
    try
    {
        {
            std::vector<double> sample_periods;
            for (const auto& channel : config.channels)
                sample_periods.push_back(channel.sample_period);
            us_per_time_ = calculateOptimalUsPerTime(sample_periods);
            SONSetFileClock(handle_, us_per_time_, 1);
        }
        for (std::size_t i{0}; i < config.channels.size(); ++i)
        {
            const auto& info{config.channels[i]};
            const auto offset_scale{calculateOffsetScale(info.min_value, info.max_value)};
            checkException(SONSetWaveChan(handle_, static_cast<short>(i), -1,
                                          static_cast<long>(std::round(info.sample_period * 1'000'000)) / us_per_time_,
                                          4096, "",
                                          getPrefix(wstringToString(info.name), SON_TITLESZ - 1).c_str(),
                                          offset_scale.scale,
                                          offset_scale.offset,
                                          getPrefix(wstringToString(info.units), SON_UNITSZ - 1).c_str()
                                          ));
            offset_scale_.push_back(offset_scale);
        }
        const auto& time_info{*gmtime(&config.start.seconds)};
        TSONTimeDate time;
        time.wYear = 1900 + time_info.tm_year;
        time.ucMon = time_info.tm_mon + 1;
        time.ucDay = time_info.tm_mday;
        time.ucHour = time_info.tm_hour;
        time.ucMin = time_info.tm_min;
        time.ucSec = time_info.tm_sec;
        time.ucHun = static_cast<uint8_t>(std::round(config.start.frac_seconds * 100));
        SONTimeDate(handle_, nullptr, &time);
        SONSetBuffering(handle_, -1, 4096 * 4);
        checkException(SONSetBuffSpace(handle_));
    }
    catch (...)
    {
        SONCloseFile(handle_);
        throw;
    }
}



void
ChannelConfig::calculateScaleInfo(const std::function<bool(std::vector<float>&)>& get_data)
{
    std::vector<float> buff;
    bool ok{get_data(buff)};
    if (!ok)
        return;
    auto minmax{std::minmax_element(buff.begin(), buff.end())};
    min_value = *minmax.first;
    max_value = *minmax.second;
    while (ok)
    {
        ok = get_data(buff);
        minmax = std::minmax_element(buff.begin(), buff.end());
        min_value = std::min(*minmax.first, min_value);
        max_value = std::max(*minmax.second, max_value);
    }
}

std::unique_ptr<FileWriter>
FileWriter::create(const Config& config)
{
    return std::unique_ptr<FileWriter>{new FileWriter(config)};
}

/*namespace
{
//thread_local
std::vector<int16_t> g_conversion_buffer;
}*/

void
FileWriter::write(short channel_id, const std::vector<float>& data)
{
    conversion_buffer_.resize(data.size());
    for (size_t i{0}; i < data.size(); ++i)
        conversion_buffer_[i] = (data[i] - offset_scale_.at(channel_id).offset)
                * 6553.6 / offset_scale_.at(channel_id).scale;
    const auto next_time{SONWriteADCBlock(handle_,
                                    channel_id,
                                          conversion_buffer_.data(),
                                          conversion_buffer_.size(),
                                    write_pos_.at(channel_id))};
    checkException(next_time);
    write_pos_.at(channel_id) = next_time;
    checkException(SONCommitFile(handle_, FALSE));
}

void
FileWriter::skipTo(Time next_time)
{
    next_time.seconds -= start_.seconds;
    next_time.frac_seconds -= start_.frac_seconds;
    if (next_time.frac_seconds < 0)
    {
        --next_time.seconds;
        next_time.frac_seconds += 1;
    }
    int64_t seconds{next_time.seconds};
    int64_t useconds{seconds * 1'000'000};
    useconds += std::round(next_time.frac_seconds * 1'000'000);
    write_pos_.assign(write_pos_.size(), useconds / us_per_time_);
}

FileWriter::~FileWriter()
{
    auto code{SONCommitFile(handle_, true)};
    if (!std::current_exception())
        checkException(code);
    code = SONCloseFile(handle_);
    if (!std::current_exception())
        checkException(code);
}

std::size_t
FileWriter::expectedSize() const
{
    long size{SONFileSize(handle_)};
    checkException(size);
    return size;
}

}
