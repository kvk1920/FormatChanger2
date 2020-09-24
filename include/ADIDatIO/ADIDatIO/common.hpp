#pragma once

#include <cmath>
#include <ctime>
#include <memory>
#include <string>
#include <vector>

namespace ADIDatIO
{

struct Time
{
    std::time_t seconds;
    double frac_seconds;

    inline
    void normalize();
};

inline
bool operator<(const Time& lhs, const Time& rhs);

using TickOffset = long;

struct ChannelTime : Time
{
    TickOffset tick_shift;
    double tick_period;

    [[nodiscard]] inline
    Time toTime() const;
};

struct RecordInfo
{
    ChannelTime start;
    std::size_t number_of_samples;
    double sample_period;
};

struct ChannelInfo
{
    std::wstring name;
    std::wstring units;
    std::vector<RecordInfo> records;
};

}

namespace ADIDatIO
{

void
Time::normalize()
{
    seconds += std::trunc(frac_seconds);
    frac_seconds -= std::trunc(frac_seconds);
}

bool operator<(const Time& lhs, const Time& rhs)
{
    if (lhs.seconds == rhs.seconds)
        return lhs.frac_seconds < rhs.frac_seconds;
    return lhs.seconds < rhs.seconds;
}

Time
ChannelTime::toTime() const
{
    Time res{seconds, frac_seconds};
    res.frac_seconds += tick_period * tick_shift;
    res.normalize();
    return res;
}

}