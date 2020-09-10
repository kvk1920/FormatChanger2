#include <Son9/file_reader.hpp>

#include <errors.hpp>

#include <extern/Son9/Son.h>

namespace Son9
{

using detail::checkException;

const char* channelTypeToString(ChannelType type)
{
    static const char* const strings[] = {
            "None",
            "ADC",
            "EventFall",
            "EventRise",
            "EventBoth",
            "Marker",
            "ADCMark",
            "RealMark",
            "TextMark",
            "RealWave",
    };
    return strings[static_cast<int>(type)];
}

double
FileInfo::secondsPerTime() const
{
    return us_per_time * base_time_unit;
}

void
FileReader::loadChannelName(ChannelInfo& channel_info, int channel_id)
{
    char buff[SON_TITLESZ + 1];
    SONGetChanTitle(handle_, channel_id, buff);
    channel_info.name = buff;
}

void
FileReader::loadChannelSamplePeriod(ChannelInfo& info, int channel_id)
{
    info.sample_period = SONChanDivide(handle_, channel_id);
    checkException(info.sample_period);
}

void
FileReader::loadADCInfo(ChannelInfo& info, int channel_id)
{
    float offset;
    float scale;
    char unit[SON_UNITSZ + 1];
    SONGetADCInfo(handle_, channel_id,
                  &scale, &offset, unit,
                  nullptr, nullptr
                  );
    info.unit.emplace(unit);
    info.offset_scale = ADC::OffsetScale{offset, scale};
}

void
FileReader::loadChannelType(ChannelInfo& info, int channel_id)
{
    switch (SONChanKind(handle_, channel_id))
    {
        case TDataKind::ChanOff:    info.type = ChannelType::None;      break;
        case TDataKind::Marker:     info.type = ChannelType::Marker;    break;
        case TDataKind::AdcMark:    info.type = ChannelType::ADCMark;   break;
        case TDataKind::RealMark:   info.type = ChannelType::RealMark;  break;
        case TDataKind::RealWave:   info.type = ChannelType::RealWave;  break;
        case TDataKind::EventBoth:  info.type = ChannelType::EventBoth; break;
        case TDataKind::EventRise:  info.type = ChannelType::EventRise; break;
        case TDataKind::EventFall:  info.type = ChannelType::EventFall; break;
        case TDataKind::TextMark:   info.type = ChannelType::TextMark;  break;
        case TDataKind::Adc:        info.type = ChannelType::ADC;       break;
        default:                    throw std::runtime_error("unknown channel type");
    }
}

namespace
{
constexpr std::size_t BLOCK_SIZE = 1024;
}

void
FileReader::loadEventTimes(std::size_t channel_id, std::vector<long>& buff)
{
    bool ok{true};
    int first_time{0};
    const int last_time{SONMaxTime(handle_)};
    int read_cnt;
    unsigned char unused;
    do
    {
        const auto old_size{buff.size()};
        buff.resize(buff.size() + BLOCK_SIZE);
        if (first_time > last_time)
            break;
        read_cnt = SONGetEventData(handle_, channel_id, buff.data() + old_size, BLOCK_SIZE,
                                   first_time, last_time, &unused, nullptr);
        buff.resize(old_size + read_cnt);
        first_time = buff.back() + 1;
    }
    while (read_cnt);
}

FileReader::FileReader(const fs::path& path)
{
    handle_ = SONOpenOldFile(path.string().c_str(), 1);
    checkException(handle_);
    info_.us_per_time = SONGetusPerTime(handle_);
    checkException(info_.us_per_time);
    info_.base_time_unit = SONTimeBase(handle_, 0.0);
    int code{SONMaxChans(handle_)};
    checkException(code);
    const int number_of_channels{code};
    info_.channels.resize(number_of_channels);
    for (int channel_id{0}; channel_id < number_of_channels; ++channel_id)
    {
        auto& channel{info_.channels[channel_id]};
        loadChannelName(channel, channel_id);
        loadChannelType(channel, channel_id);
        loadChannelSamplePeriod(channel, channel_id);
        switch (channel.type)
        {
            case ChannelType::ADC:
            case ChannelType::ADCMark:
            case ChannelType::RealWave:
                loadADCInfo(channel, channel_id);
                break;
            default: break;
        }
    }
}

struct FileReader::MakeSharedEnabler : public FileReader
{
    explicit MakeSharedEnabler(const fs::path path) : FileReader(path) {}
};

std::shared_ptr<FileReader>
FileReader::load(const fs::path& path)
{
    return std::make_shared<MakeSharedEnabler>(path);
}

}