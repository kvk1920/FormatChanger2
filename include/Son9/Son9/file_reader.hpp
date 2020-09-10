#pragma once

#include <Son9/common.hpp>

#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

namespace Son9
{

namespace fs = std::filesystem;

enum class ChannelType : int
{
    None = 0,
    ADC,
    EventFall,
    EventRise,
    EventBoth,
    Marker,
    ADCMark,
    RealMark,
    TextMark,
    RealWave,
};

const char* channelTypeToString(ChannelType type);

struct ChannelInfo
{
    ChannelType type;
    std::string name;
    long sample_period;
    std::optional<ADC::OffsetScale> offset_scale;
    std::optional<std::string> unit;
};

struct FileInfo
{
    int us_per_time;
    double base_time_unit;
    std::vector<ChannelInfo> channels;

    [[nodiscard]]
    double secondsPerTime() const;
};


class [[deprecated("untested")]] FileReader : public std::enable_shared_from_this<FileReader>
{
private:
    struct MakeSharedEnabler;
    explicit FileReader(const fs::path& path);
public:
    static std::shared_ptr<FileReader> load(const fs::path& path);
public:
    [[nodiscard]]
    const FileInfo& fileInfo() const { return info_; }

    void loadEventTimes(std::size_t channel_id, std::vector<long>& buff);
private:
    void loadChannelName(ChannelInfo& info, int channel_id);
    void loadChannelType(ChannelInfo& info, int channel_id);
    void loadChannelSamplePeriod(ChannelInfo& info, int channel_id);
    void loadADCInfo(ChannelInfo& info, int channel_id);
private:
    short handle_;
    FileInfo info_;
};

}