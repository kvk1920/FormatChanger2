#pragma once

#include <ctime>

namespace Son9
{

struct Time
{
    std::time_t seconds;
    double frac_seconds;
};

namespace ADC
{

struct OffsetScale
{
    float offset;
    float scale;
};

} // namespace ADC

} // namespace Son9