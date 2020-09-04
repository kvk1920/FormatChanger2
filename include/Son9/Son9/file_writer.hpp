#pragma once

#include <Son9/common.hpp>

#include <filesystem>

namespace Son9
{

namespace fs = std::filesystem;

struct ChannelConfig
{
    double sample_period;
    float min_value;
    float max_value;
    std::wstring name;
    std::wstring units;

    void calculateScaleInfo(const std::function<bool(std::vector<float>&)>& get_data);
};


struct Config
{
    fs::path path{};
    std::vector<ChannelConfig> channels{};
    Time start;
};

class FileWriter
{
private:
    explicit FileWriter(const Config& config);

public:
    ~FileWriter();

    [[nodiscard]]
    std::size_t numberOfChannels() const;
    [[nodiscard]]
    Time fileStart() const;
    void write(short channel_id, const std::vector<float>& data);
    void skipTo(Time next_time);

    [[nodiscard]]
    std::size_t expectedSize() const;

    static std::unique_ptr<FileWriter> create(const Config& config);

    [[nodiscard]]
    std::string path() const { return path_; }

protected:
    static ADC::OffsetScale calculateOffsetScale(float min_value, float max_value);

private:
    short handle_;
    std::vector<std::size_t> write_pos_;
    std::vector<ADC::OffsetScale> offset_scale_;
    long us_per_time_;
    Time start_;
    std::string path_;
    std::vector<int16_t> conversion_buffer_;
};


}